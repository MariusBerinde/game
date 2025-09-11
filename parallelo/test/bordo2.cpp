#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
#define YELLOW_TEXT(x) "\033[33m" x "\033[0m"
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
    int proc_row, proc_col;
    int P, Q;
    int global_rows, global_cols;
    int start_r, start_c;
    int local_rows, local_cols;

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

    reg.proc_row = rank / Q;
    reg.proc_col = rank % Q;

    split_length(global_rows, P, reg.proc_row, reg.start_r, reg.local_rows);
    split_length(global_cols, Q, reg.proc_col, reg.start_c, reg.local_cols);

    reg.cur.assign(reg.local_rows + 2, vector<int>(reg.local_cols + 2, (int)dead));
    reg.nxt.assign(reg.local_rows + 2, vector<int>(reg.local_cols + 2, (int)dead));

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

inline int count_neighbors(const GridRegion &reg, int i, int j) {
    int cnt = 0;
    for (int di = -1; di <= 1; ++di)
        for (int dj = -1; dj <= 1; ++dj)
            if (!(di == 0 && dj == 0) && reg.cur[i+di][j+dj] == (int)live)
                ++cnt;
    return cnt;
}

void compute_local_step(GridRegion &reg) {
    for (int i = 1; i <= reg.local_rows; ++i) {
        for (int j = 1; j <= reg.local_cols; ++j) {
            int n = count_neighbors(reg, i, j);
            int curv = reg.cur[i][j];
            int nxtv = (curv == (int)live)
                        ? ((n < 2 || n > 3) ? (int)dead : (int)live)
                        : ((n == 3) ? (int)live : (int)dead);
            reg.nxt[i][j] = nxtv;
        }
    }
}

// Halo exchange con Sendrecv
void halo_exchange(GridRegion &reg) {
    vector<int> buf_send, buf_recv;

    // NORTH
    if (reg.nbr_north != MPI_PROC_NULL) {
        buf_send.resize(reg.local_cols);
        buf_recv.resize(reg.local_cols);
        for (int j = 0; j < reg.local_cols; ++j) buf_send[j] = reg.cur[1][j+1];
        MPI_Sendrecv(buf_send.data(), reg.local_cols, MPI_INT, reg.nbr_north, 1,
                     buf_recv.data(), reg.local_cols, MPI_INT, reg.nbr_north, 1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int j = 0; j < reg.local_cols; ++j) reg.cur[0][j+1] = buf_recv[j];
    } else for (int j = 0; j < reg.local_cols+2; ++j) reg.cur[0][j] = (int)dead;

    // SOUTH
    if (reg.nbr_south != MPI_PROC_NULL) {
        buf_send.resize(reg.local_cols);
        buf_recv.resize(reg.local_cols);
        for (int j = 0; j < reg.local_cols; ++j) buf_send[j] = reg.cur[reg.local_rows][j+1];
        MPI_Sendrecv(buf_send.data(), reg.local_cols, MPI_INT, reg.nbr_south, 1,
                     buf_recv.data(), reg.local_cols, MPI_INT, reg.nbr_south, 1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int j = 0; j < reg.local_cols; ++j) reg.cur[reg.local_rows+1][j+1] = buf_recv[j];
    } else for (int j = 0; j < reg.local_cols+2; ++j) reg.cur[reg.local_rows+1][j] = (int)dead;

    // WEST
    if (reg.nbr_west != MPI_PROC_NULL) {
        buf_send.resize(reg.local_rows);
        buf_recv.resize(reg.local_rows);
        for (int i = 0; i < reg.local_rows; ++i) buf_send[i] = reg.cur[i+1][1];
        MPI_Sendrecv(buf_send.data(), reg.local_rows, MPI_INT, reg.nbr_west, 2,
                     buf_recv.data(), reg.local_rows, MPI_INT, reg.nbr_west, 2,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < reg.local_rows; ++i) reg.cur[i+1][0] = buf_recv[i];
    } else for (int i = 0; i < reg.local_rows+2; ++i) reg.cur[i][0] = (int)dead;

    // EAST
    if (reg.nbr_east != MPI_PROC_NULL) {
        buf_send.resize(reg.local_rows);
        buf_recv.resize(reg.local_rows);
        for (int i = 0; i < reg.local_rows; ++i) buf_send[i] = reg.cur[i+1][reg.local_cols];
        MPI_Sendrecv(buf_send.data(), reg.local_rows, MPI_INT, reg.nbr_east, 2,
                     buf_recv.data(), reg.local_rows, MPI_INT, reg.nbr_east, 2,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < reg.local_rows; ++i) reg.cur[i+1][reg.local_cols+1] = buf_recv[i];
    } else for (int i = 0; i < reg.local_rows+2; ++i) reg.cur[i][reg.local_cols+1] = (int)dead;

    // CORNERS
    auto corner_exchange = [&](int nbr, int sendv, int &dst){
        int recvv = 0;
        if (nbr != MPI_PROC_NULL) {
            MPI_Sendrecv(&sendv, 1, MPI_INT, nbr, 3,
                         &recvv, 1, MPI_INT, nbr, 3,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            dst = recvv;
        } else dst = (int)dead;
    };
    corner_exchange(reg.nbr_nw, reg.cur[1][1], reg.cur[0][0]);
    corner_exchange(reg.nbr_ne, reg.cur[1][reg.local_cols], reg.cur[0][reg.local_cols+1]);
    corner_exchange(reg.nbr_sw, reg.cur[reg.local_rows][1], reg.cur[reg.local_rows+1][0]);
    corner_exchange(reg.nbr_se, reg.cur[reg.local_rows][reg.local_cols], reg.cur[reg.local_rows+1][reg.local_cols+1]);
}

void print_global_map(int rows, int cols, const vector<int> &flat, int time, int rank) {
    if (rank != 0) return;
    cout << "Mappa tempo " << time << " (rank 0):\n";
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (flat[i * cols + j] == (int)live)
                    cout << YELLOW_TEXT("O"); 
            else
                cout << "X";                 // morto, normale
        }
        cout << "\n";
    }
}

void run_simulation(int rows, int cols, int steps, int P, int Q, vector<int> &global_flat, int rank, int size,char *name) 
{
    // Build region per rank
    GridRegion reg;
    build_region(reg, rank, size, rows, cols, P, Q);
  std::cout << "**********Start "<<name<<"*************"<<endl;

    if (rank == 0) {
        cout << "[Simulation] Regions (summary):\n";
    }
    MPI_Barrier(MPI_COMM_WORLD);
    //print_local(reg);
    MPI_Barrier(MPI_COMM_WORLD);

    // Broadcast initial grid to all ranks
    MPI_Bcast(global_flat.data(), rows * cols, MPI_INT, 0, MPI_COMM_WORLD);

    // Ogni processo carica la sua sotto-griglia locale
    for (int i = 0; i < reg.local_rows; ++i)
        for (int j = 0; j < reg.local_cols; ++j)
            reg.cur[i+1][j+1] = global_flat[(reg.start_r + i) * cols + (reg.start_c + j)];

    MPI_Barrier(MPI_COMM_WORLD);
    double tstart = now_ms();

    // Ciclo di simulazione
    for (int t = 0; t < steps; ++t) {
        // 1) scambio halo
        halo_exchange(reg);

        // 2) calcolo nuovo stato
        compute_local_step(reg);

        // 3) invio al rank 0
        int local_count = reg.local_rows * reg.local_cols;
        vector<int> sendbuf(local_count);
        for (int i = 0; i < reg.local_rows; ++i)
            for (int j = 0; j < reg.local_cols; ++j)
                sendbuf[i * reg.local_cols + j] = reg.nxt[i+1][j+1];

        int tag = 100 + t;
        if (rank == 0) {
            // Rank 0 copia la propria porzione
            for (int i = 0; i < reg.local_rows; ++i)
                for (int j = 0; j < reg.local_cols; ++j)
                    global_flat[(reg.start_r + i) * cols + (reg.start_c + j)] = sendbuf[i * reg.local_cols + j];

            // Riceve le altre porzioni
            for (int src = 1; src < size; ++src) {
                GridRegion tmp;
                build_region(tmp, src, size, rows, cols, P, Q);
                int cnt = tmp.local_rows * tmp.local_cols;
                vector<int> rbuf(cnt);
                MPI_Recv(rbuf.data(), cnt, MPI_INT, src, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int i = 0; i < tmp.local_rows; ++i)
                    for (int j = 0; j < tmp.local_cols; ++j)
                        global_flat[(tmp.start_r + i) * cols + (tmp.start_c + j)] = rbuf[i * tmp.local_cols + j];
            }

            cout << "=== Global map at step " << t << " ===\n";
            print_global_map(rows, cols, global_flat, t+1, 0);
        } else {
            MPI_Send(sendbuf.data(), local_count, MPI_INT, 0, tag, MPI_COMM_WORLD);
        }

        // 4) swap
        reg.cur.swap(reg.nxt);

        // 5) sync
        MPI_Barrier(MPI_COMM_WORLD);
    }

    double tend = now_ms();
    if (rank == 0) {
        cout << "["<<name<<"] Finished in " << (tend - tstart) << " ms\n";
    }
}

void b1(){

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // PARAMETRI FISSI (modifica qui a piacere)
    int rows  = 10;   // numero righe griglia
    int cols  = 10;   // numero colonne griglia
    int steps = 10;   // numero di iterazioni
    int P = 0, Q = 0; // suddivisione in processi (MPI_Dims_create se lasci 0)

    // Lasciamo che MPI scelga la griglia di processi
    int dims[2] = {0,0};
    MPI_Dims_create(size, 2, dims);
    P = dims[0]; 
    Q = dims[1];

    if (rank == 0) {
        cout << "[Main] Grid " << rows << "x" << cols 
             << " steps=" << steps 
             << " procs=" << size 
             << " layout=" << P << "x" << Q << endl;
    }

    // Configurazione iniziale (decisa a mano)
    vector<int> global_flat(rows * cols, (int)dead);

    if (rank == 0) {
        // esempio: Glider in alto a sinistra
        global_flat[1 * cols + 2] = (int)live;
        global_flat[2 * cols + 3] = (int)live;
        global_flat[3 * cols + 1] = (int)live;
        global_flat[3 * cols + 2] = (int)live;
        global_flat[3 * cols + 3] = (int)live;

        cout << "[Main] Initial pattern:\n";
        print_global_map(rows, cols, global_flat, 0, 0);
    }

    // Avvia la simulazione
    run_simulation(rows, cols, steps, P, Q, global_flat, rank, size,"GLIDER");
}

void bordo(){

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // PARAMETRI FISSI (modifica qui a piacere)
  int rows  = 20;   int cols  = 20;   int steps = 10;   
  int P = 0, Q = 0; // suddivisione in processi (MPI_Dims_create se lasci 0)

  // Lasciamo che MPI scelga la griglia di processi
  int dims[2] = {0,0};
  MPI_Dims_create(size, 2, dims);
  P = dims[0]; 
  Q = dims[1];

  if (rank == 0) {
    cout << "[Main] Grid " << rows << "x" << cols 
      << " steps=" << steps 
      << " procs=" << size 
      << " layout=" << P << "x" << Q << endl;
  }

  // Configurazione iniziale (decisa a mano)
  vector<int> global_flat(rows * cols, (int)dead);

  if (rank == 0) {
    // esempio: Glider in alto a sinistra

    for (int c = 0; c < cols; c++) {
      global_flat[0 * cols + c]         = (int)live;  // prima riga
      global_flat[(rows - 1) * cols + c] = (int)live; // ultima riga
    }
    for (int r = 1; r < rows - 1; r++) {
      global_flat[r * cols + 0]         = (int)live;  // prima colonna
      global_flat[r * cols + (cols - 1)] = (int)live; // ultima colonna
    }

    cout << "[Main] Initial pattern:\n";
    print_global_map(rows, cols, global_flat, 0, 0);
  }

  // Avvia la simulazione
  run_simulation(rows, cols, steps, P, Q, global_flat, rank, size,"BORDO");
}


void croce(){

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // PARAMETRI FISSI (modifica qui a piacere)
  int rows  = 20;   int cols  = 20;   int steps = 10;   
  int P = 0, Q = 0; // suddivisione in processi (MPI_Dims_create se lasci 0)

  // Lasciamo che MPI scelga la griglia di processi
  int dims[2] = {0,0};
  MPI_Dims_create(size, 2, dims);
  P = dims[0]; 
  Q = dims[1];

  if (rank == 0) {
    cout << "[Main] Grid " << rows << "x" << cols 
      << " steps=" << steps 
      << " procs=" << size 
      << " layout=" << P << "x" << Q << endl;
  }

  // Configurazione iniziale (decisa a mano)
  vector<int> global_flat(rows * cols, (int)dead);

  if (rank == 0) {
    // esempio: Glider in alto a sinistra

for (int c = 0; c < cols; c++) {
        global_flat[0 * cols + c]           = (int)live; // prima riga
        global_flat[4 * cols + c]           = (int)live;
        global_flat[9 * cols + c]           = (int)live;
        global_flat[14 * cols + c]          = (int)live;
        global_flat[(rows - 1) * cols + c]  = (int)live; // ultima riga
    }

    // Colonne vive
    for (int r = 1; r < rows - 1; r++) {
        global_flat[r * cols + 0]           = (int)live; // prima colonna
        global_flat[r * cols + 4]           = (int)live;
        global_flat[r * cols + 9]           = (int)live;
        global_flat[r * cols + 14]          = (int)live;
        global_flat[r * cols + (cols - 1)]  = (int)live; // ultima colonna
    }
    cout << "[Main] Initial pattern:\n";
    print_global_map(rows, cols, global_flat, 0, 0);
  }

  // Avvia la simulazione
  run_simulation(rows, cols, steps, P, Q, global_flat, rank, size,"BORDO");
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
  b1();
  bordo();

    MPI_Finalize();
    return 0;
}

