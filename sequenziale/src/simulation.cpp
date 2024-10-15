#include "../include/simulation.h"
//using namespace Simulation; // Implementazione della funzione toString di Nodo
std::string Nodo::toString() const {
    std::ostringstream oss;
    oss << "Nodo (" << x << ", " << y << ") stato: " << (*stato == live ? "live" : "dead");
    return oss.str();
}

// Costruttore della classe Simulation
Simulation::Simulation(int rows, int cols, int time)
    : MAX_ROWS(rows), MAX_COLS(cols), MAX_TIME(time), actual_time(0), activeNodes(time) {
    // Allocazione dinamica della mappa tridimensionale
    map = new Stato**[MAX_ROWS];
    for (int i = 0; i < MAX_ROWS; ++i) {
        map[i] = new Stato*[MAX_COLS];
        for (int j = 0; j < MAX_COLS; ++j) {
            map[i][j] = new Stato[MAX_TIME];
            for (int t = 0; t < MAX_TIME; ++t) {
                map[i][j][t] = dead;  // Inizializzazione a "dead"
            }
        }
}
}

// Distruttore della classe Simulation
Simulation::~Simulation() {
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLS; ++j) {
            delete[] map[i][j];  // Dealloca l'array di Stato
        }
        delete[] map[i];  // Dealloca l'array di puntatori alle colonne
    }
    delete[] map;  // Dealloca l'array di puntatori alle righe
}

// Funzione per aggiornare lo stato di un Nodo
void Simulation::updateNodeState(int x, int y, Stato nuovoStato, int t) {
    if (t < MAX_TIME && x < MAX_ROWS && y < MAX_COLS) {
        map[x][y][t] = nuovoStato;
        if (t == actual_time) {
            Nodo nodo = {x, y, &map[x][y][t]};
            activeNodes[t].push_back(nodo);
        }
    } else {
        std::cerr << "Errore: Indici fuori limite o tempo non valido!" << std::endl;
    }
}

// Funzione per avanzare il tempo della simulazione
void Simulation::advanceTime() {
    if (actual_time + 1 < MAX_TIME) {
        ++actual_time;
    } else {
        std::cerr << "Errore: Tempo massimo raggiunto!" << std::endl;
    }
}

// Funzione per ottenere il tempo attuale
int Simulation::getActualTime() const {
    return actual_time;
}

// Funzione per stampare tutti i nodi attivi al tempo attuale
void Simulation::printActiveNodes() const {
    std::cout << "Nodi attivi al tempo " << actual_time << ":\n";
    for (const auto& nodo : activeNodes[actual_time]) {
        std::cout << nodo.toString() << std::endl;
    }
}

// Funzione per stampare la mappa
void Simulation::printMap() const {
    std::cout << "Mappa al tempo " << actual_time << ":\n";
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLS; ++j) {
            if (map[i][j][actual_time] == live) {
                std::cout << YELLOW_TEXT("O") << " ";  // Nodo vivo ('O') in giallo
            } else {
                std::cout << " ";  // Nodo morto (spazio vuoto)
            }
        }
        std::cout << std::endl;
    }
}


std::vector<Nodo> Simulation::getNeighbours(int x,int y,int time)const{
  if (time==-1)
    time=actual_time;
  
    std::vector<Nodo> neighbours;
   // Nodo a={0,0,&map[0][0][time]};
   // neighbours.push_back(a);
    /*if(x==0 && (y!=0 && y<MAX_COLS)){
      //caso della prima riga

    }
    if(y==0 && (x!=0 && x<MAX_ROWS)){

      //caso della prima colonna

    }

    if(x==MAX_ROWS-1 && (y!=0 && y<MAX_COLS)){
      //caso dell'ultima riga

    }

    if(y==MAX_COLS-1 && (x!=0 && x<MAX_ROWS)){

      //caso dell'ultima colonna

    }
    if((y!=0 && y<=MAX_COLS) && (x!=0 && x<MAX_ROWS)){
      //caso centrale con 8 vicini
    }*/

    return neighbours;
    

  }

