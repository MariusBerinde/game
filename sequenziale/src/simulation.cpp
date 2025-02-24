#include "../include/simulation.h"

//using namespace Simulation; // Implementazione della funzione toString di Nodo
std::string Nodo::toString() const {
  std::ostringstream oss;
  oss << "Nodo (" << x << ", " << y << ") stato: " << (*stato == live ? "live" : "dead");
  return oss.str();
}

    Stato*** Simulation::getMap(){return map;}
// Costruttore della classe Simulation
Simulation::Simulation( const int rows, const int cols, const int time)
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
void Simulation::updateNodeState(const int x, const int y,const Stato nuovoStato,const int t){
  if (t < MAX_TIME && x < MAX_ROWS && y < MAX_COLS) {
    map[x][y][t] = nuovoStato;
    Nodo nodo = {x, y, &map[x][y][t]};
    activeNodes[t].push_back(nodo);

  } else {
    std::cerr << "Errore: Indici fuori limite o tempo non valido!" << std::endl;
  }
}

int Simulation::getMaxRows(){return MAX_ROWS;}
int Simulation::getMaxCols(){return MAX_COLS;}
int Simulation::getMaxTime(){return MAX_TIME;}
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
void Simulation::printMap(int p) const {
  int time=(p==-1)?actual_time:p;
    std::cout <<"Mappa al tempo " << time<< ":\n";
  for (int i = 0; i < MAX_ROWS; ++i) {
    for (int j = 0; j < MAX_COLS; ++j) {
      if (map[i][j][time] == live) {
        std::cout << YELLOW_TEXT("O"); 
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


Stato Simulation::stateNextTurn(const int x,int y) {
  auto neighbours_nodes= getNeighbours(x, y);
  Stato ris = dead;
  int nr_live_nodes=0;
  for (size_t i=0;i<neighbours_nodes.size();i++){
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

  for(size_t i=0;i<lActiveNodes.size();i++){
    auto neighbours = getNeighbours(lActiveNodes[i].x,lActiveNodes[i].y);
    for(size_t j=0;j<neighbours.size();j++){
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

bool customCompare(Nodo a, Nodo b) { return (a.x<b.x) && (a.y<b.y); }

std::vector<std::pair<int, int>> Simulation::calcSpawnNodes2(){
    std::vector<std::pair<int, int>> result;
    std::map<Point,int> map;
    auto lActiveNodes = activeNodes[actual_time];
    for(auto nodo:lActiveNodes){
      auto neighbours = getNeighbours(nodo.x,nodo.y);
      for(auto vicino:neighbours){
        if(*vicino.stato == dead){
          Point tmp_pos={vicino.x,vicino.y};
          if(map.count(tmp_pos)==1)
            map[tmp_pos]++;
          else 
            map[tmp_pos]=1;
        }
      }
    }

    for(auto e:map){

      //std::cout<<"calcSpawnNodes2["<<actual_time<<"]=("<<e.first.x<<","<<e.first.y<<")="<<e.second<<"\n";
      if(e.second==3){
        std::pair<int, int> sup (e.first.x,e.first.y);
        result.push_back(sup);
      }
    }
  return result;
  }
void Simulation::simulate_turn(){ //crea la lista nodi attivi del prossimo turno viene fatta in 2 passaggi 
  //trova quali nodi del turno attuale sopravivranno 
  //trova quali saranno i nuovi nodi attivi e inseriscili nella Mappa
  if(actual_time + 1 < MAX_TIME){
    int next_time = actual_time+1;
  //  printf("SIMULATE TURN: next time =%d\n",next_time);
    auto activeNodesNow = getActiveNodes();
    //create the active node list of the next turn
    for(size_t i=0;i<activeNodesNow.size();i++){
      auto node = activeNodesNow[i];
      if(stateNextTurn(node.x, node.y) == live ){
        updateNodeState(node.x, node.y, live, next_time);
      }
    }
    auto nodes_spawned = calcSpawnNodes2();

    for(size_t i=0;i<nodes_spawned.size();i++){

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

std::string rmSpace(std::string val){
  int non_space_count = 0;
  // Traverse a val and if it is non space character then, place it at index non_space_count
  for (int i = 0; val[i] != '\0'; i++)
    if (val[i] != ' ')
    {
      val[non_space_count] = val[i];
      non_space_count++; // non_space_count incremented
    }    

  // Finally placing final character at the val end
  val[non_space_count] = '\0';
  return val;
}

Config Simulation::read_file(const std::string& filename){

  Config ris;
  std::ifstream file(filename);
  if (!file.is_open()) {
        std::cerr << "Errore: Impossibile aprire il file di configurazione: " << filename << std::endl;
        return ris;
    }
   // std::cout << "File di configurazione aperto correttamente: " << filename << std::endl;
  // String to store each line of the file.
  std::string line;

  if (file.is_open()) {
    // Read each line from the file and store it in the
    // 'line' variable.
    while (getline(file, line)) {
      if(!line.empty() && line[0] != '/'){

        bool  sup = line.find("righe") != std::string::npos;
        if(sup){

          auto l = rmSpace(line);
          auto pos_eq=  l.find("=");
          auto pos_new_line=  l.find("\n");
          auto nrs = l.substr(pos_eq+1,pos_new_line-pos_eq-1);
          //printf("\tposizione uguale=%ld\tposzione line=%ld\n\n",pos_eq,pos_new_line);
    //      std:: cout <<"numero trovato ="<<nrs<<"\n" ;
          int nr = stoi(nrs);
          // righe = nr;
          ris.righe= nr;
        }
        sup=line.find("colonne") !=std::string::npos;
        if(sup){
          std::string l = rmSpace(line);
          auto pos_eq=  l.find("=");
          auto pos_new_line=  l.find("\n");
          std::string nrs = l.substr(pos_eq+1,pos_new_line-pos_eq-1);
          //printf("\tposizione uguale=%ld\tposzione line=%ld\n\n",pos_eq,pos_new_line);
          // cout <<"numero trovato ="<<nrs<<"\n" ;
          int nr = stoi(nrs);
          //col= nr;
          ris.colonne=nr;

        }
        sup=line.find("turni") != std::string::npos;
        if(sup){
          std::  string l = rmSpace(line);
          auto pos_eq=  l.find("=");
          auto pos_new_line=  l.find("\n");
          std::string nrs = l.substr(pos_eq+1,pos_new_line-pos_eq-1);
          //printf("\tposizione uguale=%ld\tposzione line=%ld\n\n",pos_eq,pos_new_line);
          std::  cout <<"numero trovato ="<<nrs<<"\n" ;
          int nr = stoi(nrs);
          //col= nr;
          ris.tempo=nr;

        }
        sup=line.find("Attivi") !=std:: string::npos;

        if(sup){

          while (getline(file, line)) {

            if(!line.empty() && line[0] != '/'){
              int tmp_x=-1,tmp_y=-1;
              std::   string l = rmSpace(line);
              size_t pos_1 = l.find("(");
              size_t pos_2 = l.find(",");
              size_t pos_3 = l.find(")");
              std::string nome = l.substr(0,pos_1);
              std::string sup = l.substr(pos_1+1,pos_2-pos_1);
              tmp_x = stoi(sup);
              sup = l.substr(pos_2+1,pos_3-1-pos_2);
              tmp_y = stoi(sup);
              ris.nodi.push_back({tmp_x,tmp_y,nome});
            }
          }

        }



      } 
    }

    file.close();
    /*
    ris.righe=righe; ris.colonne=col;
    ris.tempo=tempo;
    ris.nodi = nodi;*/
    return ris;
  }
  else {
    std::cerr << "Unable to open file!" <<std::endl;
  }

  return ris;
}


bool Simulation::load_config(const std::string& filename){
  Config config = read_file(filename);  // Usa la funzione read_file() da simulazione.cpp per leggere la configurazione
  /*
std::cout << "Configurazione letta: "
              << "righe = " << config.righe 
              << ", colonne = " << config.colonne 
              << ", tempo = " << config.tempo << std::endl;
*/
  MAX_ROWS = config.righe;
  MAX_COLS = config.colonne;
  MAX_TIME = config.tempo;
  actual_time = 0;

  // Alloca dinamicamente la mappa tridimensionale
  map = new Stato**[MAX_ROWS];
  for (int i = 0; i < MAX_ROWS; ++i) {
    map[i] = new Stato*[MAX_COLS];
    for (int j = 0; j < MAX_COLS; ++j) {
      map[i][j] = new Stato[MAX_TIME];
      for (int t = 0; t < MAX_TIME; ++t) {
        map[i][j][t] = dead;  // Inizializza a "dead"
      }
    }
  }

  // Imposta i nodi attivi specificati nel file di configurazione
  for (const auto& nodo : config.nodi) {
    if (nodo.x < MAX_ROWS && nodo.y < MAX_COLS) {
      updateNodeState(nodo.x, nodo.y, live, 0);  // Stato iniziale come attivo
    } else {
      std::cerr << "Errore: Nodo fuori dai limiti della griglia (" << nodo.x << ", " << nodo.y << ")\n";
    }
  }
  return true;
}

double Simulation::eu_distance_node(Nodo a,Nodo b){
  int diff_x=a.x - b.x;
  int diff_y=a.y - b.y;

  return sqrt(pow(diff_x,2)+pow(diff_y,2));
}


int Simulation::mh_distance_node(Nodo a,Nodo b){
    int diff_x=a.x - b.x;
    int diff_y=a.y - b.y;
    return abs(diff_x)+abs(diff_y);
  }


void Simulation::write_actual_sim(const std::string& filename){
  std::ofstream out_file;
  std::string tag="[write_actual_sim]:";
  //std::cout << "Apertura file "<<filename<<"\n";
  out_file.open(filename);
  //std::cout<<tag<< "Apertura file "<<filename<<"\n";
  std::string intestazione="***Simulazione Game of life***\n";
  auto nodi_attivi = activeNodes[actual_time-1];
  //out_file<<intestazione<<"Numero nodi attivi:"<<nodi_attivi.size()<<endl;
  out_file<<intestazione<<"Numero righe:"<<MAX_ROWS<<"\tNumero colonne:"<<MAX_COLS<<"\t Tempo massimo"<<MAX_TIME<<"\n"<<std::endl;
  for(int i=0;i<=actual_time;i++){
    out_file<<"Nodi attivi al tempo "<<i<<":\n"<<std::endl;
    for(auto nodo:activeNodes[i]){
      out_file<<"\t("<<nodo.x<<","<<nodo.y<<")\n";
    }
  }
  out_file.close();
  //std::cout<<tag << "Chiusura file "<<filename<<"\n";
}
