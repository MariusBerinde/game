#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <sys/time.h>
#include <chrono>
#include <thread>

#define YELLOW_TEXT(x) "\033[33m" x "\033[0m"

using namespace std;

enum Stato { dead = 0, live = 1 };

// Funzione per calcolare differenza di tempo
double tdiff(struct timeval *start, struct timeval *end) {
    return (end->tv_sec - start->tv_sec) * 1000.0 + (end->tv_usec - start->tv_usec) / 1000.0;
}

class Simulation {
private:
    int MAX_ROWS;      
    int MAX_COLS;      
    int MAX_TIME;      
    Stato ***map;      
    int actual_time;   

public:
    Simulation(int rows, int cols, int time) : MAX_ROWS(rows), MAX_COLS(cols), MAX_TIME(time), actual_time(0) {
        // Alloca mappa 3D [rows][cols][time]
        map = new Stato**[MAX_ROWS];
        for(int i = 0; i < MAX_ROWS; i++) {
            map[i] = new Stato*[MAX_COLS];
            for(int j = 0; j < MAX_COLS; j++) {
                map[i][j] = new Stato[MAX_TIME];
                // inizializza a dead
                for(int t = 0; t < MAX_TIME; ++t) map[i][j][t] = dead;
            }
        }
    }

    ~Simulation() {
        for(int i = 0; i < MAX_ROWS; i++) {
            for(int j = 0; j < MAX_COLS; j++) {
                delete[] map[i][j];
            }
            delete[] map[i];
        }
        delete[] map;
    }

    void printMap(int p, int t) const {
        if(t < 0 || t >= MAX_TIME)
            t = actual_time;
        
        if (p > -1)
            cout << "Processo[" << p << "] mappa al tempo " << t << ":\n";
        else
            cout << "Mappa al tempo " << t << ":\n";
            
        for (int i = 0; i < MAX_ROWS; ++i) {
            for (int j = 0; j < MAX_COLS; ++j) {
                if (map[i][j][t] == live) {
                    cout << YELLOW_TEXT("O"); 
                } else {
                    cout << "X";
                }
            }
            cout << "\n";
        }
    }

    // Metodi di supporto
    void setCell(int row, int col, int time, Stato state) {
        if(row >= 0 && row < MAX_ROWS && col >= 0 && col < MAX_COLS && time >= 0 && time < MAX_TIME) {
            map[row][col][time] = state;
        }
    }
    
    Stato getCell(int row, int col, int time) const {
        if(row >= 0 && row < MAX_ROWS && col >= 0 && col < MAX_COLS && time >= 0 && time < MAX_TIME) {
            return map[row][col][time];
        }
        return dead;
    }
    
    void updateNodeState(int row, int col, Stato state, int time) {
        setCell(row, col, time, state);
    }
    
    // Metodo principale per simulazione parallela con bordo
    void sim_bord();
    
    int getMaxRows() const { return MAX_ROWS; }
    int getMaxCols() const { return MAX_COLS; }
    int getMaxTime() const { return MAX_TIME; }
    int getActualTime() const { return actual_time; }
};

struct RegionMPI {
    int rank;
    int start_x, end_x, start_y, end_y;  // Coordinate globali
    int local_width, local_height;       // Dimensioni locali SENZA halo
    
    // Griglie locali con halo per tempo corrente e successivo
    vector<vector<Stato>> current_grid;
    vector<vector<Stato>> next_grid;
    
    // Vicini MPI
    int north_neighbor, south_neighbor;
    int east_neighbor, west_neighbor;
    
    RegionMPI() : rank(-1), start_x(0), end_x(0), start_y(0), end_y(0),
                  local_width(0), local_height(0),
                  north_neighbor(-1), south_neighbor(-1),
                  east_neighbor(-1), west_neighbor(-1) {}
};

void setup_region(RegionMPI& region, int rank, int MAX_ROWS, int MAX_COLS) {
    region.rank = rank;
    int mid_x = MAX_COLS / 2;
    int mid_y = MAX_ROWS / 2;
    
    switch(rank) {
        case 0: // Nord-Ovest
            region.start_y = 0; region.end_y = mid_y;
            region.start_x = 0; region.end_x = mid_x;
            region.north_neighbor = -1; region.south_neighbor = 2;
            region.west_neighbor = -1;  region.east_neighbor = 1;
            break;
            
        case 1: // Nord-Est
            region.start_y = 0;     region.end_y = mid_y;
            region.start_x = mid_x; region.end_x = MAX_COLS;
            region.north_neighbor = -1; region.south_neighbor = 3;
            region.west_neighbor = 0;   region.east_neighbor = -1;
            break;
            
        case 2: // Sud-Ovest
            region.start_y = mid_y; region.end_y = MAX_ROWS;
            region.start_x = 0;     region.end_x = mid_x;
            region.north_neighbor = 0; region.south_neighbor = -1;
            region.west_neighbor = -1; region.east_neighbor = 3;
            break;
            
        case 3: // Sud-Est
            region.start_y = mid_y; region.end_y = MAX_ROWS;
            region.start_x = mid_x; region.end_x = MAX_COLS;
            region.north_neighbor = 1; region.south_neighbor = -1;
            region.west_neighbor = 2;  region.east_neighbor = -1;
            break;
    }
    
    region.local_width = region.end_x - region.start_x;
    region.local_height = region.end_y - region.start_y;
    
    if (rank == 0 || rank==1 || rank==2 || rank==3) {
        cout << "[Rank " << rank << "] Regione: (" << region.start_y << "," << region.start_x 
             << ") -> (" << region.end_y-1 << "," << region.end_x-1 << ") "
             << "Dimensioni locali: " << region.local_height << "x" << region.local_width << endl;
    }
    
    // Alloca griglie con halo (+2 per ogni dimensione)
    region.current_grid.resize(region.local_height + 2);
    region.next_grid.resize(region.local_height + 2);
    for(int i = 0; i < region.local_height + 2; i++) {
        region.current_grid[i].resize(region.local_width + 2, dead);
        region.next_grid[i].resize(region.local_width + 2, dead);
    }
}

void load_local_data(RegionMPI& region, const Simulation& sim, int time) {
    // Copia dati dalla simulazione globale alla regione locale

    //printf("[%s]:carica dati\n",__func__);
    for(int i = 0; i < region.local_height; i++) {
        for(int j = 0; j < region.local_width; j++) {
            int global_row = region.start_y + i;
            int global_col = region.start_x + j;
            region.current_grid[i + 1][j + 1] = sim.getCell(global_row, global_col, time);
        }
    }
}

void store_local_data(const RegionMPI& region, Simulation& sim, int time) {
    // Copia dati dalla regione locale alla simulazione globale

    //printf("[%s]:  Copia dati dalla regione locale alla simulazione globale \n",__func__);
    for(int i = 0; i < region.local_height; i++) {
        for(int j = 0; j < region.local_width; j++) {
            int global_row = region.start_y + i;
            int global_col = region.start_x + j;
            
            sim.setCell(global_row, global_col, time, region.next_grid[i + 1][j + 1]);
        }
    }
}

// Helper: ritorna rank per riga/colonna della griglia 2x2, -1 se fuori
int rank_at(int row, int col) {

   // printf("[%s]:start...\n",__func__);
    if(row < 0 || row > 1 || col < 0 || col > 1) return -1;
    return row * 2 + col;
}


// Robustissimo halo_exchange basato su MPI_Sendrecv (sostituisce la versione attuale)
void halo_exchange(RegionMPI& region) {
    int rank = region.rank;

    // buffer temporanei (riutilizzati)
    vector<int> buf_send;
    vector<int> buf_recv;

    // ---------- SCAMBIO RIGHE (NORD <-> SUD) ----------
    // Nota: usiamo tag 1 per gli scambi riga/riga
    if (region.north_neighbor >= 0) {
        buf_send.assign(region.local_width, 0);
        buf_recv.assign(region.local_width, 0);
        for (int j = 0; j < region.local_width; ++j) buf_send[j] = (int)region.current_grid[1][j+1];
        MPI_Sendrecv(buf_send.data(), region.local_width, MPI_INT, region.north_neighbor, 1,
                     buf_recv.data(), region.local_width, MPI_INT, region.north_neighbor, 1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int j = 0; j < region.local_width; ++j) region.current_grid[0][j+1] = (Stato)buf_recv[j];
    } else {
        // bordo globale nord => halo morti
        for (int j = 0; j < region.local_width + 2; ++j) region.current_grid[0][j] = dead;
    }

    if (region.south_neighbor >= 0) {
        buf_send.assign(region.local_width, 0);
        buf_recv.assign(region.local_width, 0);
        for (int j = 0; j < region.local_width; ++j) buf_send[j] = (int)region.current_grid[region.local_height][j+1];
        MPI_Sendrecv(buf_send.data(), region.local_width, MPI_INT, region.south_neighbor, 1,
                     buf_recv.data(), region.local_width, MPI_INT, region.south_neighbor, 1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int j = 0; j < region.local_width; ++j) region.current_grid[region.local_height+1][j+1] = (Stato)buf_recv[j];
    } else {
        for (int j = 0; j < region.local_width + 2; ++j) region.current_grid[region.local_height+1][j] = dead;
    }

    // ---------- SCAMBIO COLONNE (WEST <-> EAST) ----------
    // Tag 2 per colonne
    if (region.west_neighbor >= 0) {
        buf_send.assign(region.local_height, 0);
        buf_recv.assign(region.local_height, 0);
        for (int i = 0; i < region.local_height; ++i) buf_send[i] = (int)region.current_grid[i+1][1];
        MPI_Sendrecv(buf_send.data(), region.local_height, MPI_INT, region.west_neighbor, 2,
                     buf_recv.data(), region.local_height, MPI_INT, region.west_neighbor, 2,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < region.local_height; ++i) region.current_grid[i+1][0] = (Stato)buf_recv[i];
    } else {
        for (int i = 0; i < region.local_height + 2; ++i) region.current_grid[i][0] = dead;
    }

    if (region.east_neighbor >= 0) {
        buf_send.assign(region.local_height, 0);
        buf_recv.assign(region.local_height, 0);
        for (int i = 0; i < region.local_height; ++i) buf_send[i] = (int)region.current_grid[i+1][region.local_width];
        MPI_Sendrecv(buf_send.data(), region.local_height, MPI_INT, region.east_neighbor, 2,
                     buf_recv.data(), region.local_height, MPI_INT, region.east_neighbor, 2,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < region.local_height; ++i) region.current_grid[i+1][region.local_width+1] = (Stato)buf_recv[i];
    } else {
        for (int i = 0; i < region.local_height + 2; ++i) region.current_grid[i][region.local_width+1] = dead;
    }

    // ---------- SCAMBIO ANGOLI (DIAGONALI) ----------
    // posizione nella griglia 2x2
    int my_row = (rank < 2) ? 0 : 1;
    int my_col = (rank % 2 == 0) ? 0 : 1;

    // tag 3 per angoli
    for (int di = -1; di <= 1; di += 2) {
        for (int dj = -1; dj <= 1; dj += 2) {
            int nbr_row = my_row + di;
            int nbr_col = my_col + dj;
            int diag_rank = -1;
            if (nbr_row >= 0 && nbr_row <= 1 && nbr_col >= 0 && nbr_col <= 1) diag_rank = nbr_row * 2 + nbr_col;

            int halo_i = (di == -1) ? 0 : region.local_height + 1;
            int halo_j = (dj == -1) ? 0 : region.local_width + 1;

            if (diag_rank == -1) {
                // bordo globale -> angolo morto
                region.current_grid[halo_i][halo_j] = dead;
                continue;
            }

            int send_i = (di == -1) ? 1 : region.local_height;
            int send_j = (dj == -1) ? 1 : region.local_width;
            int send_val = (int)region.current_grid[send_i][send_j];
            int recv_val = 0;

            MPI_Sendrecv(&send_val, 1, MPI_INT, diag_rank, 3,
                         &recv_val, 1, MPI_INT, diag_rank, 3,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            region.current_grid[halo_i][halo_j] = (Stato)recv_val;
        }
    }

    // Fine scambio halo
}
void apply_global_boundaries(RegionMPI& region) {
    // I bordi principali sono gi… applicati in halo_exchange quando non esistono vicini.
    // Qui non serve fare altro, ma lasciamo la funzione per chiarezza.
}

int count_neighbors(const RegionMPI& region, int i, int j) {
    int count = 0;
    for(int di = -1; di <= 1; di++) {
        for(int dj = -1; dj <= 1; dj++) {
            if(di == 0 && dj == 0) continue;
            if(region.current_grid[i + di][j + dj] == live) {
                count++;
            }
        }
    }
    return count;
}

void compute_next_generation(RegionMPI& region) {
    // Calcola solo celle interne (evita halo)

    //printf("[%s]:start...\n",__func__);
    for(int i = 1; i <= region.local_height; i++) {
        for(int j = 1; j <= region.local_width; j++) {
            int neighbors = count_neighbors(region, i, j);
            Stato current_state = region.current_grid[i][j];
            
            // Regole del Game of Life
            if(current_state == live) {
                if(neighbors < 2 || neighbors > 3) region.next_grid[i][j] = dead;
                else region.next_grid[i][j] = live;
            } else {
                if(neighbors == 3) region.next_grid[i][j] = live;
                else region.next_grid[i][j] = dead;
            }
        }
    }
}


void Simulation::sim_bord() {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if(size != 4) {
        if(rank == 0) {
            cout << "Errore: sim_bord() richiede esattamente 4 processi MPI\n";
        }
        return;
    }
    
    RegionMPI region;
    setup_region(region, rank, MAX_ROWS, MAX_COLS);
    
    // Prealloc buffer di invio per ogni processo (dimensione locale senza halo)
    int local_count = region.local_height * region.local_width;
    vector<int> sendbuf(local_count);
    
    // Buffer di ricezione usato da rank 0
    vector<int> recvbuf;
    
    // Simulazione per ogni tempo
    for(int t = 0; t < MAX_TIME - 1; t++) {
        actual_time = t;
        
        // 1. Carica dati dalla mappa globale alla regione locale
        load_local_data(region, *this, t);
        
        // 2. Scambia halo con processi vicini
        halo_exchange(region);
        
        // 3. Applica condizioni ai bordi globali
        apply_global_boundaries(region);
        
        // 4. Calcola prossima generazione
        compute_next_generation(region);
        
        // 5. Prepara buffer locale (solo celle interne, senza halo) per invio a rank 0
        for(int i = 0; i < region.local_height; ++i) {
            for(int j = 0; j < region.local_width; ++j) {
                sendbuf[i * region.local_width + j] = (int)region.next_grid[i+1][j+1];
            }
        }
        
        // 6. Invia i blocchi a rank 0 (e rank 0 riceve da tutti)
        int tag = 1000 + t; // tag diverso per timestep (evita confusion)
        if(rank == 0) {
            // Rank 0: prima copia il proprio blocco nella Simulation (evita recv da se stesso)
            // (oppure possiamo fare una recv anche da se stesso; qui gi… abbiamo scritto store_local_data in passato,
            // ma per sicurezza riscriviamo esplicitamente i valori di rank 0 nella Simulation)
            // Copia locale
            for(int i = 0; i < region.local_height; ++i) {
                for(int j = 0; j < region.local_width; ++j) {
                    int global_row = region.start_y + i;
                    int global_col = region.start_x + j;
                    this->setCell(global_row, global_col, t+1, (Stato)sendbuf[i * region.local_width + j]);
                }
            }
            
            // Ricevi da tutti gli altri rank e aggiorna la Simulation globale
            for(int src = 1; src < size; ++src) {
                // Ricrea la regione del mittente per calcolare posizione globale
                RegionMPI src_region;
                setup_region(src_region, src, MAX_ROWS, MAX_COLS);
                int src_count = src_region.local_height * src_region.local_width;
                recvbuf.resize(src_count);
                
                MPI_Recv(recvbuf.data(), src_count, MPI_INT, src, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                
                // Copia i dati ricevuti nella Simulation globale (tempo t+1)
                for(int i = 0; i < src_region.local_height; ++i) {
                    for(int j = 0; j < src_region.local_width; ++j) {
                        int global_row = src_region.start_y + i;
                        int global_col = src_region.start_x + j;
                        int val = recvbuf[i * src_region.local_width + j];
                        this->setCell(global_row, global_col, t+1, (Stato)val);
                    }
                }
            }
        } else {
            // Rank != 0: manda il buffer a rank 0
            MPI_Send(sendbuf.data(), local_count, MPI_INT, 0, tag, MPI_COMM_WORLD);
        }
        
        // 7. Ogni processo pu• anche salvare localmente (opzionale) nella sua Simulation: 
        //    mantiene coerenza locale ma la mappa "globale" completa Š ottenuta su rank 0.
        store_local_data(region, *this, t + 1);
        
        // 8. Swap griglie per prossima iterazione
        region.current_grid.swap(region.next_grid);
        
        // 9. Output periodico (opzionale)
        if(t % 10 == 0 && rank == 0) {
            cout << "[sim_bord] Step " << t << "/" << MAX_TIME-1 << " completato" << endl;
        }
        
        // 10. Sincronizzazione tra tutti i processi prima del prossimo step
        MPI_Barrier(MPI_COMM_WORLD);
    }
    
    actual_time = MAX_TIME - 1;
    
    if(rank == 0) {
        cout << "[sim_bord] Simulazione completata per " << MAX_TIME << " time steps" << endl;
    }
}
// Funzione ausiliaria per broadcast del pattern iniziale
void broadcast_initial_pattern(Simulation& sim, int rows, int cols) {
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    // Buffer temporaneo per il broadcast
    vector<int> pattern_data(rows * cols, 0);
    
    if(my_rank == 0) {
        // Rank 0: serializza il pattern nel buffer
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < cols; j++) {
                pattern_data[i * cols + j] = (int)sim.getCell(i, j, 0);
            }
        }
    }
    
    // Broadcast a tutti i processi
    MPI_Bcast(pattern_data.data(), rows * cols, MPI_INT, 0, MPI_COMM_WORLD);
    
    if(my_rank != 0) {
        // Altri processi: deserializzano dal buffer
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < cols; j++) {
                sim.setCell(i, j, 0, (Stato)pattern_data[i * cols + j]);
            }
        }
    }
}

void simula_bordo(){
    int my_rank, righe_teo = 20, colonne_teo = 20, turni_teo = 5;
    
    // Inizializzazione MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    int num_procs;
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    
    // Verifica che ci siano esattamente 4 processi
    if(num_procs != 4) {
        if(my_rank == 0) {
            cout << "ERRORE: simula_bordo() richiede esattamente 4 processi MPI!" << endl;
            cout << "Usa: mpirun -np 4 ./bord" << endl;
        }
        MPI_Finalize();
        exit(1);
    }
    
    if(my_rank == 0) {
        cout << "[simula_bordo] Creazione simulazione " 
             << righe_teo << "x" << colonne_teo << " per " << turni_teo << " turni" << endl;
        cout << "[simula_bordo] Usando " << num_procs << " processi MPI" << endl;
    }
    
    Simulation sim(righe_teo, colonne_teo, turni_teo);
    
    // Inizializzazione pattern solo su rank 0
    if(my_rank == 0) {
        // Pattern bordo
        for(int i = 0; i < colonne_teo; i++){
            sim.updateNodeState(i, 0, live, 0);
            sim.updateNodeState(i, colonne_teo-1, live, 0);
        }
        
        for(int i = 1; i < righe_teo-1; i++){
            sim.updateNodeState(0, i, live, 0);
            sim.updateNodeState(righe_teo-1, i, live, 0);
        }
        
        cout << "Pattern iniziale:" << endl;
        sim.printMap(my_rank, 0);
    }
    
    // Broadcast del pattern iniziale a tutti i processi
    broadcast_initial_pattern(sim, righe_teo, colonne_teo);
    
    struct timeval start, end;
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(my_rank == 0) {
        gettimeofday(&start, NULL);
    }
    
    // Esegue la simulazione parallela
    sim.sim_bord();


    
    if(my_rank == 0) {
        gettimeofday(&end, NULL);
        
        cout << endl << "=== RISULTATI FINALI ===" << endl;
    /*
        sim.printMap(my_rank, 1);

    cout<<"--------------sim 2----------\n";
        sim.printMap(my_rank, 2);
        */
        
        double execution_time = tdiff(&start, &end);
        cout << "[simula_bordo] Tempo di esecuzione: " << execution_time << " ms" << endl;
        cout << "[simula_bordo] Processi utilizzati: " << num_procs << endl;
        
        // Animazione finale (opzionale)
    
        cout << "\nVuoi vedere l'animazione? (y/n): ";
        char choice;
        cin >> choice;
        if(choice == 'y' || choice == 'Y') {
            for(int i = 0; i < turni_teo; i++) {
                cout << "\033[2J\033[H";
                cout << "Frame " << i << "/" << turni_teo-1 << endl;
                sim.printMap(my_rank, i);
                this_thread::sleep_for(chrono::milliseconds(200));
            }
        }
    
    }
  
}

// Main per test
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    simula_bordo();
    
    MPI_Finalize();
    return 0;
}
