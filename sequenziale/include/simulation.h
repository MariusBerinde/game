#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <vector>
#include <sstream>
//namespace Simulation {
// Definizioni per il colore giallo e il reset dei colori nel terminale
#define ESC "\033["
#define YELLOW_TXT "33"  // Codice ANSI per il colore giallo
#define RESET ESC << "0m"  // Reset del colore
#define YELLOW_TEXT(ARG) ESC << YELLOW_TXT << "m" << ARG << RESET

// Enum per lo stato delle celle
enum Stato { live, dead };

// Struttura Nodo
struct Nodo {
    int x, y;           // Coordinate (x, y)
    Stato* stato;       // Puntatore allo stato della cella

    // Funzione per convertire il Nodo in una stringa
    std::string toString() const;
};

// Dichiarazione della classe Simulation
class Simulation {
private:
    const int MAX_ROWS;      // Numero massimo di righe
    const int MAX_COLS;      // Numero massimo di colonne
    const int MAX_TIME;      // Numero massimo di tempi
    Stato ***map;            // Mappa tridimensionale Stato[MAX_ROWS][MAX_COLS][MAX_TIME]
    int actual_time;         // Tempo corrente della simulazione
    std::vector<std::vector<Nodo>> activeNodes;  // Nodi attivi per ogni tempo

public:
    // Costruttore
    Simulation(int rows, int cols, int time);

    // Distruttore
    ~Simulation();

    // Funzione per aggiornare lo stato di un Nodo
    void updateNodeState(int x, int y, Stato nuovoStato, int t);

    // Funzione per avanzare il tempo della simulazione
    void advanceTime();

    // Funzione per ottenere il tempo attuale
    int getActualTime() const;

    // Funzione per stampare tutti i nodi attivi al tempo attuale
    void printActiveNodes() const;

    // Funzione per stampare la mappa
    void printMap() const;


  /**
   * @brief this function returns the active nodes of the map at time
   * @param time  a int in the interval (0,MAX_TIME]
   * 
   * @return a std::vector<Nodo>
   */
  std::vector<Nodo> getActiveNodesAtTime(int time) const;

  /**
   * @brief this function returns the active nodes of the map at actual_time
   * @param time  a int in the interval (0,MAX_TIME]
   * 
   * @return a std::vector<Nodo>
   */
  std::vector<Nodo> getActiveNodes() const;

  std::vector<std::vector<Nodo>> getActiveNodesOfAnyTime() const;

    
    /**
 * @brief return a vector<Node> the neighbours of the Node rappresented by x,y at time t 
 * at t time, if t is not specified will be actual_time
 * Questa funzione prende due numeri interi come input e ritorna la loro somma.
 *
 * @param a Il primo numero intero.
 * @param b Il secondo numero intero.
 * @return La somma dei due numeri.
 */
    std::vector<Nodo> getNeighbours(int x,int y,int time=-1) const;
};

 

#endif // SIMULATION_H
 
