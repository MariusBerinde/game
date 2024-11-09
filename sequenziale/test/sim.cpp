#include <iostream>
#include <vector>
#include <sstream>
#define ESC "\033["
#define YELLOW_TXT "33"  // Codice ANSI per il colore giallo
#define RESET ESC << "0m"  // Reset del colore
#define YELLOW_TEXT(ARG) ESC << YELLOW_TXT << "m" << ARG << RESET


enum Stato { live, dead };

struct Nodo {
    int x, y;           // Coordinate (x, y)
    Stato* stato;       // Puntatore allo stato della cella

    // Funzione toString() per convertire i valori del Nodo in una stringa
    std::string toString() const {
        std::ostringstream oss;
        oss << "Nodo (" << x << ", " << y << ") stato: " << (*stato == live ? "live" : "dead");
        return oss.str();
    }
};

class Simulation {
private:
    const int MAX_ROWS;      // Numero massimo di righe (costante, una volta settata)
    const int MAX_COLS;      // Numero massimo di colonne (costante, una volta settata)
    const int MAX_TIME;      // Numero massimo di tempi (costante, una volta settata)
    Stato ***map;            // Mappa tridimensionale Stato[MAX_ROWS][MAX_COLS][MAX_TIME]
    int actual_time;         // Tempo corrente della simulazione
    std::vector<std::vector<Nodo>> activeNodes;  // Nodi attivi per ogni tempo

public:
    // Costruttore della simulazione
    Simulation(int rows, int cols, int time)
        : MAX_ROWS(rows), MAX_COLS(cols), MAX_TIME(time), actual_time(0), activeNodes(time) {
        /*
            Questo costruttore inizializza i parametri della simulazione:
            - `MAX_ROWS`: numero massimo di righe, fissato dal parametro `rows`.
            - `MAX_COLS`: numero massimo di colonne, fissato dal parametro `cols`.
            - `MAX_TIME`: numero massimo di unità temporali, fissato dal parametro `time`.
            - `actual_time`: il tempo iniziale della simulazione è impostato a 0.
            - `activeNodes`: vettore di nodi attivi per ogni unità di tempo, inizializzato con `MAX_TIME` dimensioni.
        
            La mappa tridimensionale `map` viene allocata dinamicamente:
            - `map[i][j][t]` rappresenta lo stato della cella alla posizione `(i, j)` al tempo `t`.
            Ogni cella è inizializzata con lo stato `dead`.
        */
        
        // Allocazione dinamica della mappa tridimensionale
        map = new Stato**[MAX_ROWS];
        for (int i = 0; i < MAX_ROWS; ++i) {
            map[i] = new Stato*[MAX_COLS];
            for (int j = 0; j < MAX_COLS; ++j) {
                map[i][j] = new Stato[MAX_TIME];
                // Inizializzazione delle celle con lo stato `dead`
                for (int t = 0; t < MAX_TIME; ++t) {
                    map[i][j][t] = dead;
                }
            }
        }
    }

    // Distruttore della simulazione
    ~Simulation() {
        /*
            Il distruttore si occupa di deallocare correttamente la memoria utilizzata dalla mappa tridimensionale.
            Poiché la mappa è stata allocata dinamicamente su tre livelli (triplo puntatore), dobbiamo deallocare
            ogni livello di puntatori in modo corretto, riga per riga e colonna per colonna.
        */
        for (int i = 0; i < MAX_ROWS; ++i) {
            for (int j = 0; j < MAX_COLS; ++j) {
                delete[] map[i][j];  // Dealloca array di Stato per ogni colonna
            }
            delete[] map[i];  // Dealloca array di puntatori alle colonne
        }
        delete[] map;  // Dealloca array di puntatori alle righe
    }

    // Funzione per aggiornare lo stato di un Nodo alla posizione (x, y) al tempo 't'
    void updateNodeState(int x, int y, Stato nuovoStato, int t) {
        /*
            Questa funzione aggiorna lo stato della cella alla posizione (x, y) al tempo `t` con il nuovo stato.
            Se il tempo `t` è quello attuale (`actual_time`), il nodo viene aggiunto all'elenco dei nodi attivi per
            quel tempo. È presente un controllo per assicurarsi che gli indici non superino i limiti della mappa.
        */
        if (t < MAX_TIME && x < MAX_ROWS && y < MAX_COLS) {
            map[x][y][t] = nuovoStato;

            // Aggiorna i nodi attivi solo se cambiano al tempo attuale
            if (t == actual_time) {
                Nodo nodo = {x, y, &map[x][y][t]};
                activeNodes[t].push_back(nodo);
            }
        } else {
            std::cerr << "Errore: Indici fuori limite o tempo non valido!" << std::endl;
        }
    }

    // Funzione per avanzare il tempo della simulazione
    void advanceTime() {
        /*
            Avanza il tempo della simulazione di una unità, se non si è già raggiunto il tempo massimo.
            In caso contrario, viene stampato un messaggio di errore.
        */
        if (actual_time + 1 < MAX_TIME) {
            ++actual_time;
        } else {
            std::cerr << "Errore: Tempo massimo raggiunto!" << std::endl;
        }
    }

    // Funzione per ottenere il tempo attuale
    int getActualTime() const {
        return actual_time;
    }

    // Stampa tutti i nodi attivi per il tempo attuale
    void printActiveNodes() const {
        /*
            Questa funzione stampa tutti i nodi attivi nel tempo attuale della simulazione, 
            mostrando le loro coordinate e il loro stato (live o dead).
        */
        std::cout << "Nodi attivi al tempo " << actual_time << ":\n";
        for (const auto& nodo : activeNodes[actual_time]) {
            std::cout << nodo.toString() << std::endl;
        }
    }

    void printMap() const {
      // Scorre la mappa attuale al tempo `actual_time`
      std::cout << "Mappa al tempo " << actual_time << ":\n";
      for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLS; ++j) {
          // Se il nodo è 'live', lo stampa in giallo, altrimenti lo lascia in bianco
          if (map[i][j][actual_time] == live) {
            std::cout << YELLOW_TEXT("O");  // Nodo vivo ('O') in giallo
          } else {
            std::cout << "0";  // Nodo morto (spazio vuoto)
          }
        }
        std::cout << std::endl;  // Nuova riga per la prossima riga di nodi
      }
    } 
}; 

int main() {
    // Creazione di un'istanza di Simulation con 5 righe, 5 colonne e 10 unità di tempo
    Simulation sim(5, 5, 10);

    // Aggiornamento di alcuni nodi al tempo 0
    sim.updateNodeState(1, 2, live, 0);
    sim.updateNodeState(2, 3, live, 0);
    sim.updateNodeState(4, 4, live, 0);

    // Stampa dei nodi attivi al tempo attuale
    sim.printActiveNodes();

    sim.printMap();
    // Avanzare il tempo della simulazione
    sim.advanceTime();

    // Aggiornare un nodo al nuovo tempo e stamparne lo stato
    sim.updateNodeState(3, 3, live, 1);
    sim.printActiveNodes();
    sim.printMap();

    return 0;
}

