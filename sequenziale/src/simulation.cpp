#include "../include/simulation.h"
#include <iostream>
#include <set>
#include <tuple>
#include <utility>
#include <vector>
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
      Nodo nodo = {x, y, &map[x][y][t]};
      activeNodes[t].push_back(nodo);
    
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
  if(x==0 && (y<MAX_COLS)){
    //caso della prima riga

    std::vector<Nodo> neighbours_r;
    if(y==0){
      neighbours_r.push_back({0,1,&map[0][1][time]});
      neighbours_r.push_back({1,0,&map[1][0][time]});
      neighbours_r.push_back({1,1,&map[1][1][time]});
      return neighbours_r;
    }
    if(y==MAX_COLS-1){
      neighbours_r.push_back({0,y-1,&map[0][y-1][time]});
      neighbours_r.push_back({1,y,&map[1][y][time]});
      neighbours_r.push_back({1,y-1,&map[1][y-1][time]});
      return neighbours_r;
    }

    neighbours_r.push_back({0,y-1,&map[0][y-1][time]});
    neighbours_r.push_back({0,y+1,&map[0][y+1][time]});
    neighbours_r.push_back({1,y-1,&map[1][y-1][time]});
    neighbours_r.push_back({1,y+1,&map[1][y+1][time]});
    neighbours_r.push_back({1,y,&map[1][y][time]});
    return neighbours_r;

  }
  if(x==MAX_ROWS-1 && (y<MAX_COLS)){
    //caso dell'ultima riga
    std::vector<Nodo> neighbours_r;
    if(y==0){
      neighbours_r.push_back({x-1,0,&map[x-1][0][time]});
      neighbours_r.push_back({x,1,&map[x][1][time]});
      neighbours_r.push_back({x-1,1,&map[x-1][1][time]});
      return neighbours_r;
    }

    if(y==MAX_COLS-1){
      neighbours_r.push_back({x,y-1,&map[x][y-1][time]});
      neighbours_r.push_back({x-1,y-1,&map[x-1][y-1][time]});
      neighbours_r.push_back({x,y-1,&map[x][y-1][time]});
      return neighbours_r;
    }


    neighbours_r.push_back({x,y-1,&map[x][y-1][time]});
    neighbours_r.push_back({x,y+1,&map[x][y+1][time]});
    neighbours_r.push_back({x-1,y-1,&map[x-1][y-1][time]});
    neighbours_r.push_back({x-1,y,&map[x-1][y][time]});
    neighbours_r.push_back({x-1,y+1,&map[x-1][y+1][time]});

    return neighbours_r;

  }

  if((y>0 && y<MAX_COLS-1) && (x>0 && x<MAX_ROWS-1)){
    //caso centrale con 8 vicini

    std::vector<Nodo> neighbours_r;
    // da 0,1,2
    neighbours_r.push_back({x-1,y+1,&map[x-1][y+1][time]});
    neighbours_r.push_back({x,y+1,&map[x][y+1][time]});
    neighbours_r.push_back({x+1,y+1,&map[x+1][y+1][time]});
    // nodi 3-4-5
    neighbours_r.push_back({x+1,y,&map[x+1][y][time]});
    neighbours_r.push_back({x+1,y-1,&map[x+1][y-1][time]});
    neighbours_r.push_back({x,y-1,&map[x][y-1][time]});

    // nodi 6-7
    neighbours_r.push_back({x-1,y-1,&map[x-1][y-1][time]});
    neighbours_r.push_back({x-1,y,&map[x-1][y][time]});

    return neighbours_r;

  }


  if(y==0 && (x>0 && x<MAX_ROWS-1)){

    //caso della prima colonna

    std::vector<Nodo> neighbours_r;

    neighbours_r.push_back({x-1,y,&map[x-1][y][time]});
    neighbours_r.push_back({x-1,y+1,&map[x-1][y+1][time]});
    neighbours_r.push_back({x,y+1,&map[x][y+1][time]});
    neighbours_r.push_back({x+1,y+1,&map[x+1][y+1][time]});
    neighbours_r.push_back({x+1,y,&map[x+1][y][time]});
    return neighbours_r;
  }


  if(y==MAX_COLS-1 && (x>0 && x<MAX_ROWS-1)){

    //caso dell'ultima colonna
    std::cout << "sono nel caso ultima colonna";

    std::vector<Nodo> neighbours_r;

    neighbours_r.push_back({x-1,y,&map[x-1][y][time]});
    neighbours_r.push_back({x-1,y-1,&map[x-1][y-1][time]});
    neighbours_r.push_back({x,y-1,&map[x][y-1][time]});
    neighbours_r.push_back({x+1,y-1,&map[x+1][y-1][time]});
    neighbours_r.push_back({x+1,y,&map[x+1][y][time]});
    return neighbours_r;
  }


  return neighbours;

}



std::vector<Nodo> Simulation::getActiveNodesAtTime(int time) const {
  return activeNodes[time];
}

std::vector<Nodo> Simulation::getActiveNodes()const{
  return activeNodes[actual_time];
}


std::vector<std::vector<Nodo>> Simulation::getActiveNodesOfAnyTime() const{ return activeNodes;}


Stato Simulation::stateNextTurn(int x,int y) {
  auto neighbours_nodes= getNeighbours(x, y);
  Stato ris = dead;
  int nr_live_nodes=0;
  for (int i=0;i<neighbours_nodes.size();i++){
    if ( *neighbours_nodes[i].stato == live)
      nr_live_nodes++;
  }
  ris = (nr_live_nodes == 2 || nr_live_nodes == 3)?live:dead;
  //std::cout<<"nodo di posizione: ("<<x<<","<<y<<")\t numero di vicini vivi ="<<nr_live_nodes<<"\n";
  return ris;
}


std::vector<std::pair<int, int>> Simulation::calcSpawnNodes(){
  std::vector<std::pair<int, int>> result;
  auto lActiveNodes = activeNodes[actual_time];
  std::set<std::tuple<int,int,int>,CompareTuples> candidates;
  auto add_or_update_candidate = [&](int x, int y) {
    std::tuple<int, int, int> search_tuple = std::make_tuple(x, y, 0);

    // Cerchiamo se esiste già la tupla con lo stesso x e y
    auto it = candidates.find(search_tuple);
    if (it == candidates.end()) {
      // Se non esiste, la inseriamo con il valore iniziale di 0
    //  std::cout << "Inserisco: (" << x << ", " << y << ", 0)\n";
      candidates.emplace(x, y, 1);
    } else {
      // Se esiste, rimuoviamo l'elemento attuale e lo reinseriamo con il valore aggiornato
      int current_uses = std::get<2>(*it);
      candidates.erase(it);  // Rimuove l'elemento esistente
      //std::cout << "Aggiorno: (" << x << ", " << y << ", " << current_uses + 1 << ")\n";
      candidates.emplace(x, y, current_uses + 1);  // Reinseriamo con il valore aggiornato
    }
  };

  for(int i=0;i<lActiveNodes.size();i++){
    auto neighbours = getNeighbours(lActiveNodes[i].x,lActiveNodes[i].y);
    for(int j=0;j<neighbours.size();j++){
      if( *neighbours[j].stato==dead){

        add_or_update_candidate(neighbours[j].x, neighbours[j].y);
      }
    }
  }

  for(auto it=candidates.begin();it!= candidates.end();it++){

    if(std::get<2>(*it) == 3){
     // std::cout << "Trovato: (" << std::get<0>(*it) << ", " << std::get<1>(*it) << ", " << std::get<2>(*it) << ")\n";
      std::pair<int, int> sup (std::get<0>(*it),std::get<1>(*it));
      result.push_back(sup);
    }
  }

  //std::pair<int, int> a (1,1);
  return result;

}


  void Simulation::simulate_turn(){
  //crea la lista nodi attivi del prossimo turno viene fatta in 2 passaggi 
  //trova quali nodi del turno attuale sopravivranno 
  //trova quali saranno i nuovi nodi attivi e inseriscili nella Mappa
  if(actual_time + 1 < MAX_TIME){
    int next_time = actual_time+1;
    printf("SIMULATE TURN: next time =%d\n",next_time);
    auto activeNodesNow = getActiveNodes();
    //crate the active node list of the next turn
    for(int i=0;i<activeNodesNow.size();i++){
      auto node = activeNodesNow[i];
      if(stateNextTurn(node.x, node.y) == live ){
        updateNodeState(node.x, node.y, live, next_time);
      }
    }
    auto nodes_spawned = calcSpawnNodes();
    for(int i=0;i<nodes_spawned.size();i++){

        updateNodeState(nodes_spawned[i].first, nodes_spawned[i].second, live, next_time);
    }

    advanceTime();

  }
  }


void Simulation::simulate_n_turns(int n){ 
  if(n>0 && n<MAX_TIME){
    simulate_turn();
  }
}
