//#include "../include/simulation.h"
#include <iostream>
#include <vector>
#include <string>
#include <mpi.h>

// Enum per lo stato delle celle
enum Stato { live, dead };

// Struttura Nodo
struct Nodo {
    int x, y;           // Coordinate (x, y)
    Stato *stato;       // Puntatore allo stato della cella

    // Funzione per convertire il Nodo in una stringa
    std::string toString() const {
        return "Nodo(" + std::to_string(x) + ", " + std::to_string(y) + ", Stato=" + (*stato == live ? "live" : "dead") + ")";
    }
};

// Funzione per inizializzare lo stato
Stato* initStato(Stato value) {
    Stato* stato = new Stato;
    *stato = value;
    return stato;
}



// Funzione per inizializzare lo stato

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    const int rows = 40; // Numero totale di righe
    const int cols = 40; // Numero totale di colonne
    const int rows_per_process = rows / world_size;

    // Allocazione della matrice locale per ciascun processo
    std::vector<std::vector<Nodo>> local_matrix(rows_per_process, std::vector<Nodo>(cols));

    // Inizializzazione della porzione locale della matrice
    for (int i = 0; i < rows_per_process; i++) {
        for (int j = 0; j < cols; j++) {
            int global_row = world_rank * rows_per_process + i;
            local_matrix[i][j].x = global_row;
            local_matrix[i][j].y = j;

            // Alterna lo stato tra live e dead
            if ((global_row + j) % 2 == 0) {
                  auto ptr_stato = Stato::live;
                  local_matrix[i][j].stato = initStato (live);
              } else {

                  local_matrix[i][j].stato = initStato (dead);
            }
        }
    }

    // Se necessario, raccogli i dati su un processo master
    if (world_rank == 0) {
        // Inizializzazione della matrice completa sul processo master
        std::vector<std::vector<Nodo>> complete_matrix(rows, std::vector<Nodo>(cols));

        // Copia la porzione locale del processo master nella matrice completa
        for (int i = 0; i < rows_per_process; i++) {
            for (int j = 0; j < cols; j++) {
                complete_matrix[i][j] = local_matrix[i][j];
            }
        }

        // Ricezione delle porzioni dagli altri processi
        for (int p = 1; p < world_size; p++) {
            for (int i = 0; i < rows_per_process; i++) {
                MPI_Recv(&(complete_matrix[p * rows_per_process + i][0]), cols * sizeof(Nodo), MPI_BYTE, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        // Visualizza un esempio del risultato
        std::cout << "Esempio di nodo: " << complete_matrix[0][0].toString() << std::endl;
        
        // Dealloca gli stati
	/*
        for (auto &row : complete_matrix) {
            for (auto &nodo : row) {
                delete nodo.stato;
            }
        }
	*/
    } else {
        // Invia la porzione locale al processo master
        for (int i = 0; i < rows_per_process; i++) {
            MPI_Send(&(local_matrix[i][0]), cols * sizeof(Nodo), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
        }

        // Dealloca gli stati locali
/*        for (auto &row : local_matrix) {
            for (auto &nodo : row) {
                delete nodo.stato;
            }
	    
        }
	*/
    }

    MPI_Finalize();
    //return 0;
}

