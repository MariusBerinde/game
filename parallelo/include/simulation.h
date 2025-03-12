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
#include <unordered_map>
#include <map>
#include <omp.h>
#include <cmath>
#include <sys/time.h>
#include <algorithm>
#include <random>
#include <mpi.h>
#include <bits/stdc++.h>
#include <unordered_set>

//using namespace std;
// Definizioni per il colore giallo e il reset dei colori nel terminale
#define ESC "\033["
#define YELLOW_TXT "33"  // Codice ANSI per il colore giallo
#define RESET ESC << "0m"  // Reset del colore
#define YELLOW_TEXT(ARG) ESC << YELLOW_TXT << "m" << ARG << RESET
#define TAN 2 // threshold active nodes nodi attivi nella simulazione 
#define TSN 100 // threshold system nodes (soglia minima di nodi in MPI per l'attivazione della parallelizzazione)
// Enum per lo stato delle celle

enum Stato { live, dead };
struct hash_pair {
    size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ std::hash<int>()(p.second);
    }
};

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


struct Point{
  int x,y;

  bool operator<(const Point& a) const {
    if (this->x == a.x)
      return this->y < a.y;
    return this->x < a.x;      // Confronta x
  }


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
    void updateNodeState(const int& x, const int& y, const Stato& nuovoStato,const int& t);
    //void updateNodeState(const int x, const int y, const Stato nuovoStato,const int t);

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
  const std::vector<Nodo>& getActiveNodes() const;

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

static bool customCompare(Nodo a, Nodo b) { return (a.x<b.x) && (a.y<b.y); }
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

  /*@ brief update the active nodes from the master to the other processes
   * */
  void broadcastActiveNodes();
  
  /*
   * @brief calculate witch nodes from the actual_time will live in the next turn
   * */
  void calcActualNodesNextTurn();

  /*
   
   * @brief Parallel version of calcSpawnNodes2
   * */
  void calcSpawnNodesP();


/**
 * @brief version of calcSpawnNodes where is used a hashmap for the candidate nodes
 */
  std::vector<std::pair<int, int>> calcSpawnNodes2();
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
  void simulate_turn_inv();
  void simulate_turn_inv_2();

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

  /**
  * @brief the position the active nodes in filename
  */
  void write_actual_sim(const std::string& filename="out/simulazione.txt");



std::vector<std::pair<int,int>> build_intervals(int nr_active_nodes);

};



#endif // SIMULATION_H
 
