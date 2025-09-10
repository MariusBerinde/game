#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <chrono>
#include <thread>
#include <cassert>

using namespace std;

enum Stato { dead = 0, live = 1 };

static double now_ms() {
    using namespace chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

// Utility: dividere N elementi in P partizioni (prime k get +1)
inline void split_length(int N, int P, int idx, int &start, int &len) {
    int base = N / P;
    int rem = N % P;
    if (idx < rem) {
        len = base + 1;
        start = idx * len;
    } else {
        len = base;
        start = rem * (base + 1) + (idx - rem) * base;
    }
}

struct GridRegion {
    int rank;
    int proc_row, proc_col;    // coordinate nella griglia PxQ
    int P, Q;                  // dims globali
    int global_rows, global_cols;
    int start_r, start_c;      // global start (inclusive)
    int local_rows, local_cols; // dimensioni locali (senza halo)

    // griglie con halo: (local_rows + 2) x (local_cols + 2)
    vector<vector<int>> cur;
    vector<vector<int>> nxt;

    int nbr_north, nbr_south, nbr_west, nbr_east;
    int nbr_nw, nbr_ne, nbr_sw, nbr_se;

    GridRegion() : rank(-1), proc_row(0), proc_col(0), P(0), Q(0),
                   global_rows(0), global_cols(0),
                   start_r(0), start_c(0), local_rows(0), local_cols(0),
                   nbr_north(MPI_PROC_NULL), nbr_south(MPI_PROC_NULL),
                   nbr_west(MPI_PROC_NULL), nbr_east(MPI_PROC_NULL),
                   nbr_nw(MPI_PROC_NULL), nbr_ne(MPI_PROC_NULL),
                   nbr_sw(MPI_PROC_NULL), nbr_se(MPI_PROC_NULL) {}
};

void build_region(GridRegion &reg, int rank, int size, int global_rows, int global_cols, int P, int Q) {
    reg.rank = rank;
    reg.global_rows = global_rows;
    reg.global_cols = global_cols;
    reg.P = P; reg.Q = Q;

    // compute proc row/col from rank layout row-major
    reg.proc_row = rank / Q;
    reg.proc_col = rank % Q;

    // split rows among P, cols among Q
    split_length(global_rows, P, reg.proc_row, reg.start_r, reg.local_rows);
    split_length(global_cols, Q, reg.proc_col, reg.start_c, reg.local_cols);

    // allocate halos (+2)
    reg.cur.assign(reg.local_rows + 2, vector<int>(reg.local_cols + 2, (int)dead));
    reg.nxt.assign(reg.local_rows + 2, vector<int>(reg.local_cols + 2, (int)dead));

    // compute neighbor ranks (or MPI_PROC_NULL if outside)
    // north: (proc_row-1, proc_col)
    auto proc_rank = [&](int r, int c) -> int {
        if (r < 0 || r >= P || c < 0 || c >= Q) return MPI_PROC_NULL;
        return r * Q + c;
    };
    reg.nbr_north = proc_rank(reg.proc_row - 1, reg.proc_col);
    reg.nbr_south = proc_rank(reg.proc_row + 1, reg.proc_col);
    reg.nbr_west  = proc_rank(reg.proc_row, reg.proc_col - 1);
    reg.nbr_east  = proc_rank(reg.proc_row, reg.proc_col + 1);
    reg.nbr_nw = proc_rank(reg.proc_row - 1, reg.proc_col - 1);
    reg.nbr_ne = proc_rank(reg.proc_row - 1, reg.proc_col + 1);
    reg.nbr_sw = proc_rank(reg.proc_row + 1, reg.proc_col - 1);
    reg.nbr_se = proc_rank(reg.proc_row + 1, reg.proc_col + 1);
}

// Copia dalla mappa globale (solo rank 0 possiede la mappa completa) alla regione locale (time t)
void load_local(GridRegion &reg, const vector<int> &global_flat, int t, int total_time) {
    // global_flat is rows*cols flattened row-major for time t, only used on rank 0 to broadcast initial pattern.
    // In our program we will use a broadcast of initial grid to all procs upfront (so each process can load from sim representation)
    // But here we assume caller provides correct flat buffer for current time.
    for (int i = 0; i < reg.local_rows; ++i) {
        for (int j = 0; j < reg.local_cols; ++j) {
            int gr = reg.start_r + i;
            int gc = reg.start_c + j;
            reg.cur[i+1][j+1] = global_flat[gr * reg.global_cols + gc];
        }
    }
}

// count neighbors using reg.cur (halo filled)
inline int count_neighbors(const GridRegion &reg, int i, int j) {
    // i and j are indices in local grid including halo (1..local_rows, 1..local_cols)
    int cnt = 0;
    for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
            if (di == 0 && dj == 0) continue;
            if (reg.cur[i+di][j+dj] == (int)live) ++cnt;
        }
    }
    return cnt;
}

void compute_local_step(GridRegion &reg) {
    for (int i = 1; i <= reg.local_rows; ++i) {
        for (int j = 1; j <= reg.local_cols; ++j) {
            int n = count_neighbors(reg, i, j);
            int curv = reg.cur[i][j];
            int nxtv = (int)dead;
            if (curv == (int)live) {
                nxtv = (n < 2 || n > 3) ? (int)dead : (int)live;
            } else {
                nxtv = (n == 3) ? (int)live : (int)dead;
            }
            reg.nxt[i][j] = nxtv;
        }
    }
}

// Robust halo exchange using MPI_Sendrecv (rows, cols, corners). Buffers are reused.
void halo_exchange(GridRegion &reg) {
    // Temporary buffers
    vector<int> buf_send;
    vector<int> buf_recv;

    // ----- exchange NORTH <-> this -----
    if (reg.nbr_north != MPI_PROC_NULL) {
        buf_send.resize(reg.local_cols);
        buf_recv.resize(reg.local_cols);
        for (int j = 0; j < reg.local_cols; ++j) buf_send[j] = reg.cur[1][j+1];
        MPI_Sendrecv(buf_send.data(), reg.local_cols, MPI_INT, reg.nbr_north, 1,
                     buf_recv.data(), reg.local_cols, MPI_INT, reg.nbr_north, 1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int j = 0; j < reg.local_cols; ++j) reg.cur[0][j+1] = buf_recv[j];
    } else {
        for (int j = 0; j < reg.local_cols + 2; ++j) reg.cur[0][j] = (int)dead;
    }

    // ----- exchange SOUTH -----
    if (reg.nbr_south != MPI_PROC_NULL) {
        buf_send.resize(reg.local_cols);
        buf_recv.resize(reg.local_cols);
        for (int j = 0; j < reg.local_cols; ++j) buf_send[j] = reg.cur[reg.local_rows][j+1];
        MPI_Sendrecv(buf_send.data(), reg.local_cols, MPI_INT, reg.nbr_south, 1,
                     buf_recv.data(), reg.local_cols, MPI_INT, reg.nbr_south, 1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int j = 0; j < reg.local_cols; ++j) reg.cur[reg.local_rows+1][j+1] = buf_recv[j];
    } else {
        for (int j = 0; j < reg.local_cols + 2; ++j) reg.cur[reg.local_rows+1][j] = (int)dead;
    }

    // ----- exchange WEST -----
    if (reg.nbr_west != MPI_PROC_NULL) {
        buf_send.resize(reg.local_rows);
        buf_recv.resize(reg.local_rows);
        for (int i = 0; i < reg.local_rows; ++i) buf_send[i] = reg.cur[i+1][1];
        MPI_Sendrecv(buf_send.data(), reg.local_rows, MPI_INT, reg.nbr_west, 2,
                     buf_recv.data(), reg.local_rows, MPI_INT, reg.nbr_west, 2,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < reg.local_rows; ++i) reg.cur[i+1][0] = buf_recv[i];
    } else {
        for (int i = 0; i < reg.local_rows + 2; ++i) reg.cur[i][0] = (int)dead;
    }

    // ----- exchange EAST -----
    if (reg.nbr_east != MPI_PROC_NULL) {
        buf_send.resize(reg.local_rows);
        buf_recv.resize(reg.local_rows);
        for (int i = 0; i < reg.local_rows; ++i) buf_send[i] = reg.cur[i+1][reg.local_cols];
        MPI_Sendrecv(buf_send.data(), reg.local_rows, MPI_INT, reg.nbr_east, 2,
                     buf_recv.data(), reg.local_rows, MPI_INT, reg.nbr_east, 2,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < reg.local_rows; ++i) reg.cur[i+1][reg.local_cols+1] = buf_recv[i];
    } else {
        for (int i = 0; i < reg.local_rows + 2; ++i) reg.cur[i][reg.local_cols+1] = (int)dead;
    }

    // ----- exchange corners -----
    // NW
    {
        int sendv = reg.cur[1][1];
        int recvv = 0;
        if (reg.nbr_nw != MPI_PROC_NULL) {
            MPI_Sendrecv(&sendv, 1, MPI_INT, reg.nbr_nw, 3,
                         &recvv, 1, MPI_INT, reg.nbr_nw, 3,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            reg.cur[0][0] = recvv;
        } else {
            reg.cur[0][0] = (int)dead;
        }
    }
    // NE
    {
        int sendv = reg.cur[1][reg.local_cols];
        int recvv = 0;
        if (reg.nbr_ne != MPI_PROC_NULL) {
            MPI_Sendrecv(&sendv, 1, MPI_INT, reg.nbr_ne, 3,
                         &recvv, 1, MPI_INT, reg.nbr_ne, 3,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            reg.cur[0][reg.local_cols+1] = recvv;
        } else {
            reg.cur[0][reg.local_cols+1] = (int)dead;
        }
    }
    // SW
    {
        int sendv = reg.cur[reg.local_rows][1];
        int recvv = 0;
        if (reg.nbr_sw != MPI_PROC_NULL) {
            MPI_Sendrecv(&sendv, 1, MPI_INT, reg.nbr_sw, 3,
                         &recvv, 1, MPI_INT, reg.nbr_sw, 3,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            reg.cur[reg.local_rows+1][0] = recvv;
        } else {
            reg.cur[reg.local_rows+1][0] = (int)dead;
        }
    }
    // SE
    {
        int sendv = reg.cur[reg.local_rows][reg.local_cols];
        int recvv = 0;
        if (reg.nbr_se != MPI_PROC_NULL) {
            MPI_Sendrecv(&sendv, 1, MPI_INT, reg.nbr_se, 3,
                         &recvv, 1, MPI_INT, reg.nbr_se, 3,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            reg.cur[reg.local_rows+1][reg.local_cols+1] = recvv;
        } else {
            reg.cur[reg.local_rows+1][reg.local_cols+1] = (int)dead;
        }
    }
}

void print_local(const GridRegion &reg) {
    cout << "[Rank " << reg.rank << "] Regione: (" << reg.start_r << "," << reg.start_c << ") -> ("
         << (reg.start_r + reg.local_rows - 1) << "," << (reg.start_c + reg.local_cols - 1) << ") "
         << " local=" << reg.local_rows << "x" << reg.local_cols << endl;
}

void print_global_map(int rows, int cols, const vector<int> &flat, int time, int rank) {
    if (rank != 0) return;
    cout << "Mappa tempo " << time << " (rank 0):\n";
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cout << (flat[i * cols + j] == (int)live ? 'O' : 'X');
        }
        cout << "\n";
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 4) {
        if (rank == 0) {
            cerr << "Usage: " << argv[0] << " <rows> <cols> <steps> [P Q]\n";
        }
        MPI_Finalize();
        return 1;
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    int steps = atoi(argv[3]);

    int P = 0, Q = 0;
    if (argc >= 6) {
        P = atoi(argv[4]);
        Q = atoi(argv[5]);
        if (P * Q != size) {
            if (rank == 0) cerr << "Errore: P*Q must equal number of processes\n";
            MPI_Finalize();
            return 1;
        }
    } else {
        // lascia che MPI suggerisca dims
        int dims[2] = {0,0};
        MPI_Dims_create(size, 2, dims);
        P = dims[0];
        Q = dims[1];
    }

    if (rank == 0) {
        cout << "[Main] Grid " << rows << "x" << cols << " steps=" << steps << " procs=" << size << " layout=" << P << "x" << Q << endl;
    }

    // Build region per rank
    GridRegion reg;
    build_region(reg, rank, size, rows, cols, P, Q);
    if (rank == 0) {
        cout << "[Main] Regions (summary):\n";
    }
    MPI_Barrier(MPI_COMM_WORLD);
    print_local(reg);
    MPI_Barrier(MPI_COMM_WORLD);

    // Initialize global pattern on rank 0 and broadcast initial grid
    vector<int> global_flat(rows * cols, (int)dead);
    if (rank == 0) {
        // Example: border alive, interior dead (like before)
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (r == 0 || r == rows-1 || c == 0 || c == cols-1) global_flat[r * cols + c] = (int)live;
                else global_flat[r * cols + c] = (int)dead;
            }
        }
        cout << "[Main] Initial pattern (rank 0):\n";
        print_global_map(rows, cols, global_flat, 0, 0);
    }

    // Broadcast initial grid to all ranks so they can initialize their local cur
    MPI_Bcast(global_flat.data(), rows * cols, MPI_INT, 0, MPI_COMM_WORLD);

    // Each rank loads its local block (time 0)
    for (int i = 0; i < reg.local_rows; ++i)
        for (int j = 0; j < reg.local_cols; ++j)
            reg.cur[i+1][j+1] = global_flat[(reg.start_r + i) * cols + (reg.start_c + j)];

    MPI_Barrier(MPI_COMM_WORLD);
    double tstart = now_ms();

    // Simulation loop
    for (int t = 0; t < steps; ++t) {
        // 1) exchange halos
        halo_exchange(reg);

        // 2) compute local next
        compute_local_step(reg);

        // 3) prepare send buffer (no halo), send to rank 0
        int local_count = reg.local_rows * reg.local_cols;
        vector<int> sendbuf(local_count);
        for (int i = 0; i < reg.local_rows; ++i)
            for (int j = 0; j < reg.local_cols; ++j)
                sendbuf[i * reg.local_cols + j] = reg.nxt[i+1][j+1];

        int tag = 100 + t;
        if (rank == 0) {
            // rank 0 writes its own block, then receives from others
            for (int i = 0; i < reg.local_rows; ++i)
                for (int j = 0; j < reg.local_cols; ++j)
                    global_flat[(reg.start_r + i) * cols + (reg.start_c + j)] = sendbuf[i * reg.local_cols + j];

            // receive from others
            for (int src = 0; src < size; ++src) {
                if (src == 0) continue;
                // derive region for src
                GridRegion tmp;
                build_region(tmp, src, size, rows, cols, P, Q);
                int cnt = tmp.local_rows * tmp.local_cols;
                vector<int> rbuf(cnt);
                MPI_Recv(rbuf.data(), cnt, MPI_INT, src, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // copy rbuf into global_flat
                for (int i = 0; i < tmp.local_rows; ++i)
                    for (int j = 0; j < tmp.local_cols; ++j)
                        global_flat[(tmp.start_r + i) * cols + (tmp.start_c + j)] = rbuf[i * tmp.local_cols + j];
            }
            // optionally print global
            cout << "=== Global map at step " << t << " ===\n";
            print_global_map(rows, cols, global_flat, t+1, 0);
        } else {
            MPI_Send(sendbuf.data(), local_count, MPI_INT, 0, tag, MPI_COMM_WORLD);
        }

        // 4) swap local buffers for next iter
        reg.cur.swap(reg.nxt);

        // 5) synchronize
        MPI_Barrier(MPI_COMM_WORLD);
    }

    double tend = now_ms();
    if (rank == 0) {
        cout << "[Main] Simulation finished in " << (tend - tstart) << " ms\n";
    }

    MPI_Finalize();
    return 0;
}
