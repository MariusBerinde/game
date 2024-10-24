#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <utility>
#include <vector>
#include <sstream>
#include <set>
#include <tuple>
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

struct CompareTuples {
    bool operator()(const std::tuple<int, int, int>& lhs, const std::tuple<int, int, int>& rhs) const {
        // Confronta solo x e y (primi due elementi della tupla)
        return std::tie(std::get<0>(lhs), std::get<1>(lhs)) < std::tie(std::get<0>(rhs), std::get<1>(rhs));
    }
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


  /**
   * @brief return the state of the node of position x,y 
   * @param x : x position of node
   * @param y : y position of node
   *
   * @return state of the node for the next turn
  */
  Stato stateNextTurn(int x,int y) ;
/**
   * @brief calc the spawn position for the new nodes in the map 
   *
   * @return a vector of pairs where the first in the pair is the x, and second is the y
  */
  std::vector<std::pair<int, int>> calcSpawnNodes();

  /**
   * @brief Updates the states of the map to the next shift according to the following rules: 
   * - If a node has 3 or 2 neighbors ; the node will live on the next turn otherwise it will be considered dead 
   * - If a dead node has exactly 3 live neighbors ; the node will be considered live in the next turn
   **/
  void simulate_turn();

  /**
   * @brief run simulate_turn n times
  */
  void simulate_n_turns(int n);
};

 

#endif // SIMULATION_H
 
