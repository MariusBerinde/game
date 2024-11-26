#include "../include/simulation.h"
#include <algorithm>
#include <cstddef>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <vector>
using namespace std;
void test_send(){
  int my_rank,size;
   MPI_Comm_size(MPI_COMM_WORLD,&size);
    if(size!=2){
        printf("Errore: il numero di processi deve essere 3");
    		MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
	 }
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
  else{
        Simulation sim(6, 6, 10);
        int count;
        MPI_Recv(&count, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::vector<int> buffer(3 * count);
        MPI_Recv(buffer.data(), buffer.size(), MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int time_received = 0;
        for (int i = 0; i < count; ++i) {
            int x = buffer[3 * i];
            int y = buffer[3 * i + 1];
            Stato stato = static_cast<Stato>(buffer[3 * i + 2]);
            sim.updateNodeState(x, y,stato ,  time_received);
        }

        const auto nodes = sim.getActiveNodes();
        std::cout << "Processo " << my_rank << ": Tempo " << time_received << " ricevuto con " << nodes.size() << " nodi.\n";
        for (const auto& nodo : nodes) {
            std::cout << "  " << nodo.toString() << "\n";
        }
  }

	MPI_Finalize();
}



// Funzione per inviare e ricevere la matrice
void send_matrix() {

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
}

/*test condivisione */
void test_share(){
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
      printf("Processo[%d](%d)\tnumero di nodi attivi %d\n",my_rank,sim.getActualTime(),sim.getActiveNodes().size());
      sim.simulate_turn();
      sim.printMap(my_rank);
      printf("Processo[%d](%d)\t ho eseguito un tunrno e adesso numero di nodi attivi %d\n",my_rank,sim.getActualTime(),sim.getActiveNodes().size());
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
}
void test_scheduler(){

  Nodo b = {5, 5, NULL};
  Nodo a = {0, 0, NULL};
  Nodo c = {1, 3, NULL};
  Nodo d = {3, 2, NULL};
  Nodo e = {0, 2, NULL};
  Nodo f = {4, 2, NULL};

  vector<Nodo> l={a,b,c,d,e,f};

    int my_rank,size,TAG=0;
   MPI_Comm_size(MPI_COMM_WORLD,&size);
    if(size!=3){
        printf("Errore: il numero di processi deve essere 3");
    		MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
	 }
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);      
    if(my_rank==0){
      int start_1=0, start_2=3, end_1=2, end_2=5,d1=1,d2=2;
    std::cout << "Nodi all'inizio\n";
    for(Nodo n:l){
      printf("(%d,%d)\t",n.x,n.y);
    }
    std::cout << "\n";
      MPI_Send(&start_1,1,MPI_INT,d1,TAG,MPI_COMM_WORLD);
      MPI_Send(&end_1,1,MPI_INT,d1,TAG,MPI_COMM_WORLD);
      MPI_Send(&start_2,1,MPI_INT,d2,TAG,MPI_COMM_WORLD);
      MPI_Send(&end_2,1,MPI_INT,d2,TAG,MPI_COMM_WORLD);

        MPI_Recv(&start_1, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&start_2, 1, MPI_INT, 2, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if(start_1==42 && start_2==42)
          cout<<"Sincronizzazione\n";

        std::cout << "Nodi dopo il lavoro";
        for(Nodo n:l){
          printf("(%d,%d)\t",n.x,n.y);
        }

        std::cout << "\n";
    }
    else{
      int start=0,end=0;

        MPI_Recv(&start, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Nodo[%d] start=%d\tend=%d\n",my_rank,start,end);
        for (size_t i = start; i <= end ; i++) {
          l[i].x++;
          l[i].y++;
        }
       start=42;

      MPI_Send(&start,1,MPI_INT,0,TAG,MPI_COMM_WORLD);

    }
}


void test_scheduler_2() {
    Nodo b = {5, 5, NULL};
    Nodo a = {0, 0, NULL};
    Nodo c = {1, 3, NULL};
    Nodo d = {3, 2, NULL};
    Nodo e = {0, 2, NULL};
    Nodo f = {4, 2, NULL};

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
        for (size_t i = start; i <= end; i++) {
            l[i].x++;
            l[i].y++;
        }

        // Invia i nodi aggiornati al master
        MPI_Send(&l[start], (end - start + 1) * sizeof(Nodo), MPI_BYTE, 0, TAG, MPI_COMM_WORLD);
    }
}

int main(int argc,char *argv[]){
  MPI_Init(&argc, &argv);
	//test_send();
  //send_matrix();
  //test_share();
//  test_scheduler();
  test_scheduler_2();
}

