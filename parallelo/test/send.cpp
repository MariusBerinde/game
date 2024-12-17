#include "../include/simulation.h"
#include <mpi.h>
using namespace std;
float tdiff(struct timeval *start,struct timeval *end){
  return (end->tv_sec-start->tv_sec) + 1e-6 * (end->tv_usec-start->tv_usec);
}
void test_send(){

  MPI_Init(NULL, NULL);
  int my_rank,size;
   MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);   
  int TAG=0;
  
  if(my_rank == 0){
    Simulation sim(6, 6, 2);
    sim.updateNodeState(0, 0, live, 0); //A
    sim.updateNodeState(0, 1, live, 0); //B
    sim.updateNodeState(1, 0, live, 0); //G
    const auto nodi_attivi = sim.getActiveNodes();
    size_t count = nodi_attivi.size();
    vector<int> buffer(3*count);
    for (size_t i=0; i<count; i++) {
      buffer[3*i]=nodi_attivi[i].x;
      buffer[3*i+1]=nodi_attivi[i].y;
      buffer[3 * i + 2] = (nodi_attivi[i].stato != nullptr) ? static_cast<int>(*(nodi_attivi[i].stato)) : static_cast<int>(dead);
    }
    MPI_Send(&count,1,MPI_INT,1,TAG,MPI_COMM_WORLD);
    MPI_Send(buffer.data(),buffer.size(),MPI_INT,1,TAG,MPI_COMM_WORLD); 
    
    cout<<"processo "<< my_rank <<"ha inviato i nodi\n";
    
  }

	MPI_Finalize();
}

// Funzione per inviare e ricevere la matrice
void send_matrix() {

    MPI_Init(NULL, NULL);
    int my_rank,size;
   MPI_Comm_size(MPI_COMM_WORLD,&size);
    if(size!=3){
        printf("Errore: il numero di processi deve essere 3");
    		MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
	 }
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);      const int TAG = 0;

    if (my_rank == 0) {
        // Processo root: genera la matrice e la invia
        Simulation sim(6, 6, 2);
        sim.updateNodeState(0, 0, live, 0); // A
        sim.updateNodeState(0, 1, live, 0); // B
        sim.updateNodeState(1, 0, live, 0); // G

        const int max_rows = sim.getMaxRows();
        const int max_cols = sim.getMaxCols();
        const int mid_col = max_cols / 2;

        std::cout << "Processo principale ecco la matrice degli stati al tempo 0:\n";
        auto matrice = sim.getMap();
        for (int i = 0; i < max_rows; ++i) {
            for (int j = 0; j < max_cols; ++j) {
                printf("m[%d][%d]=%d\t", i, j, matrice[i][j][0]);
            }
            std::cout << "\n";
        }

        // Invia met… matrice ai processi 1 e 2
        for (int target_rank = 1; target_rank <= 2; ++target_rank) {
            int start_col = (target_rank == 1) ? 0 : mid_col;
            int col_count = (target_rank == 1) ? mid_col : (max_cols - mid_col);

            // Invia dimensioni
            MPI_Send(&max_rows, 1, MPI_INT, target_rank, TAG, MPI_COMM_WORLD);
            MPI_Send(&col_count, 1, MPI_INT, target_rank, TAG, MPI_COMM_WORLD);

            // Invia righe
            std::vector<int> buffer(col_count);
            for (int i = 0; i < max_rows; ++i) {
                for (int j = 0; j < col_count; ++j) {
                    buffer[j] = static_cast<int>(matrice[i][start_col + j][0]);
                }
                MPI_Send(buffer.data(), buffer.size(), MPI_INT, target_rank, TAG, MPI_COMM_WORLD);
            }
        }
    } else {
        // Processi 1 e 2: ricevono i dati
        int max_rows, col_count;
        MPI_Recv(&max_rows, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&col_count, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Alloca la matrice locale
        Stato** local_map = new Stato*[max_rows];
        for (int i = 0; i < max_rows; ++i) {
            local_map[i] = new Stato[col_count];
        }

        // Ricevi i dati riga per riga
        std::vector<int> buffer(col_count);
        for (int i = 0; i < max_rows; ++i) {
            MPI_Recv(buffer.data(), buffer.size(), MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0; j < col_count; ++j) {
                local_map[i][j] = static_cast<Stato>(buffer[j]);
            }
        }

        // Stampa la matrice locale ricevuta
        std::cout << "Sono il processo " << my_rank << " e ho ricevuto la mia parte della matrice:\n";
        for (int i = 0; i < max_rows; ++i) {
            for (int j = 0; j < col_count; ++j) {
                printf("m[%d][%d]=%d\t", i, j, local_map[i][j]);
            }
            std::cout << "\n";
        }

        // Libera la memoria
        for (int i = 0; i < max_rows; ++i) {
            delete[] local_map[i];
        }
        delete[] local_map;
    }
    MPI_Finalize();
}

/*test condivisione */
void test_share(){
   MPI_Init(NULL, NULL);
    int my_rank,size;
   MPI_Comm_size(MPI_COMM_WORLD,&size);
    if(size!=3){
        printf("Errore: il numero di processi deve essere 3");
    		MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
	 }
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);      
        Simulation sim(6, 6, 2);
        sim.updateNodeState(0, 0, live, 0); // A
        sim.updateNodeState(0, 1, live, 0); // B
        sim.updateNodeState(1, 0, live, 0); // G
    if (my_rank == 0) {
      printf("Processo[%d](%d)\tnumero di nodi attivi %ld\n",my_rank,sim.getActualTime(),sim.getActiveNodes().size());
      sim.simulate_turn();
      sim.printMap(my_rank);
      printf("Processo[%d](%d)\t ho eseguito un tunrno e adesso numero di nodi attivi %ld\n",my_rank,sim.getActualTime(),sim.getActiveNodes().size());
      sim.printMap(my_rank);
    }

    if (my_rank == 1) {
      auto nodi_attivi = sim.getActiveNodes();
      printf("Processo[%d](%d)\tnumero di nodi attivi %ld\n",my_rank,sim.getActualTime(),sim.getActiveNodes().size());
      printf("Processo[%d] stampo mappa\n",my_rank);
      sim.printMap(my_rank);
    }

    if (my_rank == 2) {
      auto nodi_attivi = sim.getActiveNodes();
      printf("Processo[%d](%d)\tnumero di nodi attivi %ld\n",my_rank,sim.getActualTime(),sim.getActiveNodes().size());
      sim.printMap(my_rank);
    }


      // sim.simulate_turn();
/*
    if (my_rank == 0) {
      printf("Processo[%d]\tnumero di nodi attivi %d\n",my_rank,sim.getActualTime());
      sim.simulate_turn();
      printf("Processo[%d]\tnumero di nodi attivi %d\n",my_rank,sim.getActualTime());
    }

    if (my_rank == 1) {
      printf("Processo[%d]\tnumero di nodi attivi %d\n",my_rank,sim.getActualTime());
    }

    if (my_rank == 2) {
      printf("Processo[%d]\tnumero di nodi attivi %d\n",my_rank,sim.getActualTime());
    }

    */

    MPI_Finalize();
}



void test_scheduler() {
    Nodo b = {5, 5, NULL};
    Nodo a = {0, 0, NULL};
    Nodo c = {1, 3, NULL};
    Nodo d = {3, 2, NULL};
    Nodo e = {0, 2, NULL};
    Nodo f = {4, 2, NULL};
    std::vector<Nodo> l = {a, b, c, d, e, f};
    MPI_Init(NULL, NULL);
    int my_rank, size, TAG = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != 3) {
        printf("Errore: il numero di processi deve essere 3");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    if (my_rank == 0) {
        // Processo master
        int start_1 = 0, start_2 = 3, end_1 = 2, end_2 = 5, d1 = 1, d2 = 2;

        std::cout << "Nodi all'inizio\n";
        for (Nodo n : l) {
            printf("(%d,%d)\t", n.x, n.y);
        }
        std::cout << "\n";

        // Invia intervalli ai figli
        MPI_Send(&start_1, 1, MPI_INT, d1, TAG, MPI_COMM_WORLD);
        MPI_Send(&end_1, 1, MPI_INT, d1, TAG, MPI_COMM_WORLD);
        MPI_Send(&start_2, 1, MPI_INT, d2, TAG, MPI_COMM_WORLD);
        MPI_Send(&end_2, 1, MPI_INT, d2, TAG, MPI_COMM_WORLD);

        // Ricevi i nodi aggiornati dai figli
        MPI_Recv(&l[start_1], (end_1 - start_1 + 1) * sizeof(Nodo), MPI_BYTE, d1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&l[start_2], (end_2 - start_2 + 1) * sizeof(Nodo), MPI_BYTE, d2, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::cout << "Nodi dopo il lavoro\n";
        for (Nodo n : l) {
            printf("(%d,%d)\t", n.x, n.y);
        }
        std::cout << "\n";

    } else {
        // Processi figli
        int start = 0, end = 0;
        // Ricevi intervallo dal master
        MPI_Recv(&start, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Nodo[%d] start=%d\tend=%d\n", my_rank, start, end);
        // Modifica i nodi assegnati
        for (int i = start; i <= end; i++) {
            l[i].x++;
            l[i].y++;
        }
        // Invia i nodi aggiornati al master
        MPI_Send(&l[start], (end - start + 1) * sizeof(Nodo), MPI_BYTE, 0, TAG, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}

void test_scheduler_b() {
    Nodo b = {5, 5, NULL};
    Nodo a = {0, 0, NULL};
    Nodo c = {1, 3, NULL};
    Nodo d = {3, 2, NULL};
    Nodo e = {0, 2, NULL};
    Nodo f = {4, 2, NULL};
    MPI_Init(NULL, NULL);
    std::vector<Nodo> l = {a, b, c, d, e, f};
    int my_rank, size, TAG = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != 3) {
        printf("Errore: il numero di processi deve essere 3");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    if (my_rank == 0) {
        // Processo master
        int start_1 = 0, start_2 = 3, end_1 = 2, end_2 = 5, d1 = 1, d2 = 2;

        // Mostra lo stato iniziale dei nodi
        std::cout << "Master:nodi all'inizio\n";
        for (Nodo n : l) {
            printf("(%d,%d)\t", n.x, n.y);
        }
        std::cout << "\n";

        // Invia intervalli ai figli
        MPI_Send(&start_1, 1, MPI_INT, d1, TAG, MPI_COMM_WORLD);
        MPI_Send(&end_1, 1, MPI_INT, d1, TAG, MPI_COMM_WORLD);
        MPI_Send(&start_2, 1, MPI_INT, d2, TAG, MPI_COMM_WORLD);
        MPI_Send(&end_2, 1, MPI_INT, d2, TAG, MPI_COMM_WORLD);

        // Ricevi i nodi aggiornati dai figli
        MPI_Recv(&l[start_1], (end_1 - start_1 + 1) * sizeof(Nodo), MPI_BYTE, d1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&l[start_2], (end_2 - start_2 + 1) * sizeof(Nodo), MPI_BYTE, d2, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Propaga la lista aggiornata a tutti i processi
        MPI_Bcast(&l[0], l.size() * sizeof(Nodo), MPI_BYTE, 0, MPI_COMM_WORLD);

        std::cout << "Master:nodi dopo il lavoro:\n";
        for (Nodo n : l) {
            printf("Master:(%d,%d)\t", n.x, n.y);
        }
        std::cout << "\n";

    } else {
        // Processi figli
        int start = 0, end = 0;

        // Ricevi intervallo dal master
        MPI_Recv(&start, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

       // printf("Nodo[%d] start=%d\tend=%d\n", my_rank, start, end);

        // Modifica i nodi assegnati
        for (int i = start; i <= end; i++) {
            l[i].x++;
            l[i].y++;
        }
        // Invia i nodi aggiornati al master
        MPI_Send(&l[start], (end - start + 1) * sizeof(Nodo), MPI_BYTE, 0, TAG, MPI_COMM_WORLD);
        // Propaga la lista aggiornata dal master
        MPI_Bcast(&l[0], l.size() * sizeof(Nodo), MPI_BYTE, 0, MPI_COMM_WORLD);
        // Mostra i nodi aggiornati
        std::cout << "Nodi dopo la propagazione (figlio " << my_rank << ")\n";
        for (Nodo n : l) {
            printf("Processo[%d]:(%d,%d)\t",my_rank,n.x, n.y);
        }
        std::cout << "\n";
    }
    MPI_Finalize();
}

void test_scheduler_b_2() {
    Nodo b = {5, 5, NULL};
    Nodo a = {0, 0, NULL};
    Nodo c = {1, 3, NULL};
    Nodo d = {3, 2, NULL};
    Nodo e = {0, 2, NULL};
    Nodo f = {4, 2, NULL};
    MPI_Init(NULL, NULL);
    std::vector<Nodo> l = {a, b, c, d, e, f};
    int my_rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (size != 3) {
        printf("Errore: il numero di processi deve essere 3\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    int total_nodes = l.size();
    int nodes_per_process = total_nodes / (size - 1); // Numero di nodi per figlio
    int remainder = total_nodes % (size - 1);        // Restanti nodi non divisibili
    if (my_rank == 0) {
        std::cout << "Master: nodi all'inizio\n";
        for (Nodo n : l) {
            printf("(%d,%d)\t", n.x, n.y);
        }
        std::cout << "\n";

        // Notifica ai figli di iniziare il calcolo
        for (int i = 1; i < size; i++) {
            int start = (i - 1) * nodes_per_process;
            int end = start + nodes_per_process - 1;
            if (i == size - 1) {
                end += remainder; // L'ultimo figlio gestisce i nodi rimanenti
            }

            MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&end, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Il master attende notifiche di completamento da tutti i figli
        for (int i = 1; i < size; i++) {
            int done;
            MPI_Recv(&done, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Notifica che i dati sono stati aggiornati
        std::cout << "Master: tutti i dati sono stati aggiornati\n";
        // Mostra lo stato finale dei nodi
        std::cout << "Master: nodi dopo l'aggiornamento\n";
        for (Nodo n : l) {
            printf("Master:(%d,%d)\t", n.x, n.y);
        }
        std::cout << "\n";
    } else {
        // FIGLI
        int start, end;
        // Riceve i limiti del lavoro
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Aggiorna la porzione assegnata
        for (int i = start; i <= end; i++) {
            l[i].x += my_rank; // Simula un aggiornamento dipendente dal rank
            l[i].y += my_rank;
        }
        // Propaga i dati aggiornati
        MPI_Bcast(&l[0], total_nodes * sizeof(Nodo), MPI_BYTE, my_rank, MPI_COMM_WORLD);

        // Notifica al master che ha terminato
        int done = 1;
        MPI_Send(&done, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        // Mostra i nodi aggiornati localmente
        std::cout << "Processo[" << my_rank << "]: nodi dopo l'aggiornamento\n";
        for (Nodo n : l) {
            printf("Processo[%d]:(%d,%d)\t", my_rank, n.x, n.y);
        }
        std::cout << "\n";
    }

    MPI_Finalize();
}


void Simulation::simulate_turn_p() {
    int my_rank, size,i=0;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int next_time = actual_time + 1;

    // Parte del processo principale (rank 0)
    if (my_rank == 0) {
      i++;

      std::cout << "["<<my_rank<<"] e i vale ="<<i<<"\n";
        std::vector<Nodo> activeNodesNow = getActiveNodes();
        // Ordinare e dividere i nodi tra i processi
        sort(activeNodesNow.begin(), activeNodesNow.end(), Simulation::customCompare);

        int chunk_size = activeNodesNow.size() / (size - 1);
        for (int i = 1; i < size; ++i) {
            int start = (i - 1) * chunk_size;
            int end = (i == size - 1) ? activeNodesNow.size() - 1 : start + chunk_size - 1;

            // Inviare gli intervalli a ogni processo
            MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&end, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

            // Inviare i dati dei nodi dell'intervallo
            for (int j = start; j <= end; ++j) {
                Nodo& node = activeNodesNow[j];
                MPI_Send(&node.x, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&node.y, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        }

        // Raccogliere gli aggiornamenti dagli altri processi
        std::vector<std::tuple<int, int, Stato>> updates;
        for (int i = 1; i < size; ++i) {
            int num_updates;
            MPI_Recv(&num_updates, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int j = 0; j < num_updates; ++j) {
                int x, y;
                Stato updated_state;
                MPI_Recv(&x, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&y, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&updated_state, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                updates.emplace_back(x, y, updated_state);
            }
        }

        // Inviare gli aggiornamenti a tutti i processi
        int total_updates = updates.size();
        MPI_Bcast(&total_updates, 1, MPI_INT, 0, MPI_COMM_WORLD);
        for (auto& [x, y, updated_state] : updates) {
            MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&y, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&updated_state, 1, MPI_INT, 0, MPI_COMM_WORLD);
        }

        // Applicare gli aggiornamenti localmente
        for (const auto& [x, y, updated_state] : updates) {
            updateNodeState(x, y, updated_state, next_time);
        }
    } else {

      std::cout << "["<<my_rank<<"] e i vale ="<<i<<"\n";
        // Parte degli altri processi
        std::vector<Nodo> activeNodesNow = getActiveNodes();
        int start, end;
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::vector<std::tuple<int, int, Stato>> local_updates;

        for (int i = start; i <= end; ++i) {
            Nodo node = activeNodesNow[i];
            if (stateNextTurn(node.x, node.y) == live) {
                updateNodeState(node.x, node.y, live, next_time);
                local_updates.emplace_back(node.x, node.y, live);
            }
        }

        // Inviare gli aggiornamenti al processo principale
        int num_updates = local_updates.size();
        MPI_Send(&num_updates, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        for (const auto& [x, y, updated_state] : local_updates) {
            MPI_Send(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&updated_state, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }

        // Ricevere la lista globale degli aggiornamenti
        int total_updates;
        MPI_Bcast(&total_updates, 1, MPI_INT, 0, MPI_COMM_WORLD);
        for (int i = 0; i < total_updates; ++i) {
            int x, y;
            Stato updated_state;
            MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&y, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&updated_state, 1, MPI_INT, 0, MPI_COMM_WORLD);
            updateNodeState(x, y, updated_state, next_time);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    // Avanzare al tempo successivo
    if (my_rank == 0) {
      i++;
      std::cout << "["<<my_rank<<"] e i vale ="<<i<<"\n";

       // advanceTime();
    }
    else{
      i++;
      std::cout << "["<<my_rank<<"] e i vale ="<<i<<"\n";
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

void test_rules_next_turn(){

    Simulation sim(6, 6, 10);
    sim.updateNodeState(0, 0, live, 0); //A0 
    sim.updateNodeState(0, 1, live, 0); //B1
    sim.updateNodeState(4, 1, live, 0); //C2
    sim.updateNodeState(5, 0, live, 0); //D3
    sim.updateNodeState(5, 2, live, 0); //E4
    sim.updateNodeState(2, 0, live, 0); //F5
    sim.updateNodeState(1, 0, live, 0); //G6
    sim.updateNodeState(1, 1, live, 0); //H7
    std::cout<<"TEST RULES NEXT TURN\n";
    //sim.printMap();
    auto neighBoursA = sim.getNeighbours(0, 0);
    auto neighBoursB = sim.getNeighbours(0, 1);
    auto neighBoursC = sim.getNeighbours(4, 1);
    auto neighBoursD = sim.getNeighbours(5, 0);
    auto neighBoursE = sim.getNeighbours(5, 2);
    auto neighBoursF = sim.getNeighbours(1, 0);
    auto neighBoursG = sim.getNeighbours(1, 0);
    auto neighBoursH = sim.getNeighbours(1, 1);

    struct timeval start,end;
    gettimeofday(&start, NULL);
    sim.simulate_turn();
    gettimeofday(&end, NULL);

  std::cout << "Test creation\n";
  printf("velocità di init con parallizzazione  millisec %0.6f\n",tdiff(&start, &end));
    vector<Nodo> listaNodiAttivi=sim.getActiveNodes();
    //if(sim.stateNextTurn(0,0)==live)
    if(*listaNodiAttivi[0].stato==live)
      std::cout<<"Stato nodo A ok\tnumero vicini="<<neighBoursA.size()<<"\n";
    else {
      std::cout<<"Stato nodo A problema\tnumero vicini="<<neighBoursA.size()<<"\n";
    }

    //if(sim.stateNextTurn(0,0)==live)
    if(*listaNodiAttivi[1].stato==live)
      std::cout<<"Stato nodo B ok\tnumero vicini="<<neighBoursB.size()<<"\n";
    else {
      std::cout<<"Stato nodo B problema\tnumero vicini="<<neighBoursB.size()<<"\n";
    }

    //if(sim.stateNextTurn(0,0)==live)
    if(*listaNodiAttivi[2].stato==live)
      std::cout<<"Stato nodo C ok\tnumero vicini="<<neighBoursC.size()<<"\n";
    else {
      std::cout<<"Stato nodo C problema\tnumero vicini="<<neighBoursC.size()<<"\n";
    }
    /*
       if(sim.stateNextTurn(5,0)==dead)
       std::cout<<"Stato nodo D ok\tnumero vicini="<<neighBoursD.size()<<"\n";
       else {
       std::cout<<"Stato nodo D problema\tnumero vicini="<<neighBoursD.size()<<"\n";
       }
       if(sim.stateNextTurn(5,2)==dead)
       std::cout<<"Stato nodo E ok\tnumero vicini="<<neighBoursE.size()<<"\n";
       else {
       std::cout<<"Stato nodo E problema\tnumero vicini="<<neighBoursE.size()<<"\n";
       }
       */
    //if(sim.stateNextTurn(0,0)==live)
    if(*listaNodiAttivi[3].stato==live)
      std::cout<<"Stato nodo F ok\tnumero vicini="<<neighBoursF.size()<<"\n";
    else {
      std::cout<<"Stato nodo F problema\tnumero vicini="<<neighBoursF.size()<<"\n";
    }
    /*
       if(sim.stateNextTurn(1,0)==dead)
       std::cout<<"Stato nodo G ok\tnumero vicini="<<neighBoursG.size()<<"\n";
       else {
       std::cout<<"Stato nodo G problema\tnumero vicini="<<neighBoursG.size()<<"\n";
       }
       if(sim.stateNextTurn(1,1)==dead)
       std::cout<<"Stato nodo H ok\tnumero vicini="<<neighBoursH.size()<<"\n";
       else {
       std::cout<<"Stato nodo H problema\tnumero vicini="<<neighBoursH.size()<<"\n";
       }*/

}

void test_rules_next_turn_2() {
    int my_rank;
  MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Inizializzazione della simulazione
    Simulation sim(6, 6, 10);
    sim.updateNodeState(0, 0, live, 0); // A
    sim.updateNodeState(0, 1, live, 0); // B
    sim.updateNodeState(4, 1, live, 0); // C
    sim.updateNodeState(5, 0, live, 0); // D
    sim.updateNodeState(5, 2, live, 0); // E
    sim.updateNodeState(2, 0, live, 0); // F
    sim.updateNodeState(1, 0, live, 0); // G
    sim.updateNodeState(1, 1, live, 0); // H

    if (my_rank == 0) {
        std::cout << "TEST RULES NEXT TURN\n";
    }

    // Misurazione del tempo (solo processo master)
    struct timeval start, end;
    if (my_rank == 0) {
        gettimeofday(&start, NULL);
    }

    // Esegui la simulazione parallela
    sim.simulate_turn_p();

    // Misurazione del tempo (solo processo master)
    if (my_rank == 0) {
        gettimeofday(&end, NULL);
        printf("Tempo di esecuzione con parallelismo: %0.6f ms\n", tdiff(&start, &end));
    }

    // Controllo del risultato (solo processo master)
    if (my_rank == 0) {
        auto listaNodiAttivi = sim.getActiveNodes();

        // Test dei nodi
        auto check_node = [](const Nodo& nodo, const std::string& nome, size_t vicini, bool stato_atteso) {
            std::cout << "Stato nodo " << nome << (stato_atteso == (*nodo.stato == live) ? " OK" : " Problema")
                      << "\tNumero vicini=" << vicini << "\n";
        };

        auto neighBoursA = sim.getNeighbours(0, 0);
        auto neighBoursB = sim.getNeighbours(0, 1);
        auto neighBoursC = sim.getNeighbours(4, 1);
        auto neighBoursF = sim.getNeighbours(2, 0);

        check_node(listaNodiAttivi[0], "A", neighBoursA.size(), true);
        check_node(listaNodiAttivi[1], "B", neighBoursB.size(), true);
        check_node(listaNodiAttivi[2], "C", neighBoursC.size(), true);
        check_node(listaNodiAttivi[3], "F", neighBoursF.size(), true);
    }

    MPI_Finalize();
}
int main(int argc,char *argv[]){
 // MPI_Init(&argc, &argv);
//	test_send();
 // send_matrix();
  //test_share();
  //test_scheduler();
  //test_scheduler_b();
  //test_scheduler_b_2();
  //test_rules_next_turn();
  test_rules_next_turn_2();
}

