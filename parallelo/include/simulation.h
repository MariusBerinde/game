#ifndef SIMULATION_H
#define SIMULATION_H

#include <bits/types/struct_timeval.h>
#include <cstdio>
#include <iostream>
#include <utility>
#include <vector>
#include <sstream>
#include <fstream>
#include <set>
#include <tuple>
#include <string>
#include <mpi.h>
#include <omp.h>
#include <cmath>
#include <sys/time.h>
#include <algorithm>
#include <random>

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


struct Pos {
  int x,y;
  std::string nome;
};

struct Config{
  int righe,colonne,tempo;
  std::vector<Pos> nodi;
};




// Dichiarazione della classe Simulation
class Simulation {
private:
    int MAX_ROWS;      // Numero massimo di righe
    int MAX_COLS;      // Numero massimo di colonne
    int MAX_TIME;      // Numero massimo di tempi
    Stato ***map;            // Mappa tridimensionale Stato[MAX_ROWS][MAX_COLS][MAX_TIME]
    int actual_time;         // Tempo corrente della simulazione
    std::vector<std::vector<Nodo>> activeNodes;  // Nodi attivi per ogni tempo

public:
    // Costruttore
    Simulation(int rows, int cols, int time);
    Simulation();

  // creazione simulazione da file
    Simulation(const std::string& filename);

    // Distruttore
    ~Simulation();

    int getMaxRows();
    int getMaxCols();
    int getMaxTime();

    Stato*** getMap();

    // Funzione per aggiornare lo stato di un Nodo
    void updateNodeState(int x, int y, Stato nuovoStato, int t);

    // Funzione per avanzare il tempo della simulazione
    void advanceTime();

    // Funzione per ottenere il tempo attuale
    int getActualTime() const;

    // Funzione per stampare tutti i nodi attivi al tempo attuale
    void printActiveNodes() const;

    // Funzione per stampare la mappa p indica il processo 
    void printMap(int p=-1) const;


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

  /*
   * @brief parallel version of sumulate turn
   * */
  void simulate_turn_p();

  /**
   * @brief run simulate_turn n times
  */
  void simulate_n_turns(int n);



  /**
   * @brief read the configuration from the file filename
   *
  */
  Config read_file(const std::string& filename);
  


  /**
   * @brief load the configuration from the filename
   *
  */
  bool load_config(const std::string& filename);

  /**
   * @brief calc the euclidean distance between a and b
   * */
  double eu_distance_node(Nodo a,Nodo b);

  /**
   * @brief calc the manhattan distance between a and b
   * */
  int mh_distance_node(Nodo a,Nodo b);


  };

 

#endif // SIMULATION_H
 
