#include "../include/simulation.h"

std::string Nodo::toString() const {
  std::ostringstream oss;
  oss << "Nodo (" << x << ", " << y << ") stato: " << (*stato == live ? "live" : "dead");
  return oss.str();
}

    Stato*** Simulation::getMap(){return map;}

// Costruttore della classe Simulation
Simulation::Simulation(int rows, int cols, int time)
  : MAX_ROWS(rows), MAX_COLS(cols), MAX_TIME(time), actual_time(0), activeNodes(time) { // Allocazione dinamica della mappa tridimensionale
  map = new Stato**[MAX_ROWS];
    #pragma omp parallel for 
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

  if (p> -1)
    std::cout <<"Processo["<<p<< "] mappa al tempo " << actual_time << ":\n";
  else
    std::cout <<"Mappa al tempo " << actual_time << ":\n";

  for (int i = 0; i < MAX_ROWS; ++i) {
    for (int j = 0; j < MAX_COLS; ++j) {
      if (map[i][j][actual_time] == live) {
        std::cout << YELLOW_TEXT("O"); 
      } else {
        std::cout << "X";  // Nodo morto (spazio vuoto)
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
    //std::cout << "sono nel caso ultima colonna";

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

  for ( size_t i=0;i<lActiveNodes.size();i++){
    auto neighbours = getNeighbours(lActiveNodes[i].x,lActiveNodes[i].y);
    for ( size_t j=0;j<neighbours.size();j++){
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

//bool customCompare(Nodo a, Nodo b) { return (a.x<b.x) && (a.y<b.y); }

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
/*
 * strategia di parallelizzazione :
 * il nodo con rank == 0 distribuisce il lavoro restanti nodi per cui nella fase iniziale ci devono essere almeno 3 nodi per poter lavorare 
 * TODO: capire come parallelizzare le operazioni di regola 1,2 e 3
 * avendo size nodi ci saranno size-1 nodi figli che verificheranno 
 * */
/*
  void Simulation::simulate_turn_p(){
    int my_rank,size,TAG;
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);   

    size--; // diminuisco il totale perc
    if(my_rank==0){

      //
      - nodo scheduler 
      - ordina i nodi attivi 
      -  lo scheduler ordina la lista dei nodi attivi e distriubisce gli elenchi gli intervalli ai nodi figli dopo di che si mette in attesa del done dei nodi figli per poter attivare la fase di spawn nodes e far avanzare il tempo totale della simulazione 
      //

       
      auto activeNodesNow = getActiveNodes();
      sort(activeNodesNow.begin(),activeNodesNow.end(),customCompare);
      //TODO: selezionare i vari indici e inviarli ai nodi giusti
    }
    else {
      //nodo figli

    }

      int next_time = actual_time+1;
      printf("SIMULATE TURN: next time =%d\n",next_time);
      auto activeNodesNow = getActiveNodes();
      //crate the active node list of the next turn
      for ( size_t i=0;i<activeNodesNow.size();i++){
        auto node = activeNodesNow[i];
        if(stateNextTurn(node.x, node.y) == live ){
          updateNodeState(node.x, node.y, live, next_time);
        }
      }
      auto nodes_spawned = calcSpawnNodes();
      for ( size_t i=0;i<nodes_spawned.size();i++){

        updateNodeState(nodes_spawned[i].first, nodes_spawned[i].second, live, next_time);
      }
      //std::cout<<"simula turno numero di nodi attivi="<<activeNodesNow.size()<<"\t valore par :"<<activeNodes[next_time].size()<<"\n";

      advanceTime();
    
  }
*/

/*
void Simulation::simulate_turn_p() {
    int my_rank, size,i=0;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int next_time = actual_time + 1;
    // Parte calcolo nodi attivi al prossimo turno
    // Parte del processo principale (rank 0)
    if (my_rank == 0) {
    i++;
    std::cout << "["<<my_rank<<"] e i vale ="<<i<<"\n";
    std::vector<Nodo> activeNodesNow = getActiveNodes();
    // Ordinare e dividere i nodi tra i processi
    sort(activeNodesNow.begin(), activeNodesNow.end(), Simulation::customCompare);
    int chunk_size = activeNodesNow.size() / (size - 1);
    for (int i = 1; i < size; ++i) {
      int start = (i - 1) * chunk_size;
      int end = (i == size - 1) ? activeNodesNow.size() - 1 : start + chunk_size - 1;
      std::pair<int,int> range(start,end);

      // Inviare gli intervalli a ogni processo
      MPI_Send(&range, 2, MPI_INT, i, 0, MPI_COMM_WORLD);

      // Inviare i dati dei nodi dell'intervallo
      for (int j = start; j <= end; ++j) {
        Nodo& node = activeNodesNow[j];
        MPI_Send(&node.x, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&node.y, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      }
    }

    // Raccogliere gli aggiornamenti dagli altri processi
    std::vector<std::tuple<int, int, Stato>> updates;
    for (int i = 1; i < size; ++i) {
      int num_updates;
      MPI_Recv(&num_updates, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      for (int j = 0; j < num_updates; ++j) {
        int x, y;
        Stato updated_state;
        MPI_Recv(&x, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&y, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&updated_state, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        updates.emplace_back(x, y, updated_state);
      }
    }

    // Inviare gli aggiornamenti a tutti i processi
    int total_updates = updates.size();
    MPI_Bcast(&total_updates, 1, MPI_INT, 0, MPI_COMM_WORLD);
    for (auto& [x, y, updated_state] : updates) {
      MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Bcast(&y, 1, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Bcast(&updated_state, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Applicare gli aggiornamenti localmente
    for (const auto& [x, y, updated_state] : updates) {
      updateNodeState(x, y, updated_state, next_time);
    }
    } else {

      std::cout << "["<<my_rank<<"] e i vale ="<<i<<"\n";
        // Parte degli altri processi
        std::vector<Nodo> activeNodesNow = getActiveNodes();
        std::pair<int, int> r;// r.first is the start of the range and r.second is the end of the range
        MPI_Recv(&r, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::vector<std::tuple<int, int, Stato>> local_updates;

        for (int i = r.first; i <= r.second; ++i) {
            Nodo node = activeNodesNow[i];
            if (stateNextTurn(node.x, node.y) == live) {
                updateNodeState(node.x, node.y, live, next_time);
                local_updates.emplace_back(node.x, node.y, live);
            }
        }

        // Inviare gli aggiornamenti al processo principale
        int num_updates = local_updates.size();
        MPI_Send(&num_updates, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        for (const auto& [x, y, updated_state] : local_updates) {
            MPI_Send(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&updated_state, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }

        // Ricevere la lista globale degli aggiornamenti
        int total_updates;
        MPI_Bcast(&total_updates, 1, MPI_INT, 0, MPI_COMM_WORLD);
        for (int i = 0; i < total_updates; ++i) {
            int x, y;
            Stato updated_state;
            MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&y, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&updated_state, 1, MPI_INT, 0, MPI_COMM_WORLD);
            updateNodeState(x, y, updated_state, next_time);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
   //computazione dei nodi spawnati nel prossimo turno 
    if (my_rank == 0) {
      i++;
      std::cout << "["<<my_rank<<"] e i vale ="<<i<<"\n";

      //std::vector<std::pair<int, int>> result;
      std::map<Point,int> map;

        std::vector<Nodo> activeNodesNow = getActiveNodes();
        // Ordinare e dividere i nodi tra i processi
        sort(activeNodesNow.begin(), activeNodesNow.end(), Simulation::customCompare);

        int chunk_size = activeNodesNow.size() / (size - 1);
        for (int i = 1; i < size; ++i) {
            int start = (i - 1) * chunk_size;
            int end = (i == size - 1) ? activeNodesNow.size() - 1 : start + chunk_size - 1;
            std::pair<int, int> range(start,end);
            // Inviare gli intervalli a ogni processo
            MPI_Send(&range, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
            // Inviare i dati dei nodi dell'intervallo
      
//            for (int j = start; j <= end; ++j) {
//                Nodo& node = activeNodesNow[j];
//                MPI_Send(&node.x, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
//                MPI_Send(&node.y, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
//            }

        }
    // ricezione dei dati della grande hashmap
      for(int i=1;i<size;i++){
        int buf_size;
        MPI_Recv(&buf_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::vector<int> buffer(buf_size);
        MPI_Recv(buffer.data(), buf_size, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (size_t i = 0; i < buffer.size(); i += 3) {
            Point pos = {buffer[i], buffer[i + 1]};
            int value = buffer[i + 2];
            if(map.count(pos)==1)
              map[pos]+=value;
            else 
              map[pos]=value;
            //cout<<"Master :Pos("<<pos.x<<","<<pos.y<<")="<<value<<"\tmap="<<map.at(pos)<<"\n";
        }
      }

    for(auto e:map){

      //std::cout<<"calcSpawnNodes2["<<actual_time<<"]=("<<e.first.x<<","<<e.first.y<<")="<<e.second<<"\n";
      if(e.second==3){
        std::pair<int, int> sup (e.first.x,e.first.y);
        //result.push_back(sup);
        
        updateNodeState(e.first.x,e.first.y, live, next_time);
      }
    }


    }
    else{
    i++;
    std::cout << "["<<my_rank<<"] e i vale ="<<i<<"\n";
    //ricevono gli intervalli di lavoro e restituiscono i risultati 
    std::vector<Nodo> activeNodesNow = getActiveNodes();

    std::pair<int,int> range;
    int start=range.first,end=range.second;
    MPI_Recv(&range, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::map<Point,int> map_l;
    for(int i=start;i<=end;i++){
      auto neighbours = getNeighbours(activeNodesNow[i].x,activeNodesNow[i].y);
      for(auto vicino:neighbours){
        if(*vicino.stato == dead){
          Point tmp_pos={vicino.x,vicino.y};
          if(map_l.count(tmp_pos)==1)
            map_l[tmp_pos]++;
          else 
            map_l[tmp_pos]=1;
        }
      }
    }

        // invia map a master
    
    std::vector<int> buffer;
          for (const auto& e : map_l) {
            buffer.push_back(e.first.x);
            buffer.push_back(e.first.y);
            buffer.push_back(e.second);
          }
          // Invio del numero di elementi (dimensione del buffer)
          int buffer_size = buffer.size();

          MPI_Send(&buffer_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
          MPI_Send(buffer.data(), buffer_size, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);

   advanceTime();
}
*/
/*
 * crea la lista nodi attivi del prossimo turno viene fatta in 2 passaggi 
 * trova quali nodi del turno attuale sopravivranno 
 * trova quali saranno i nuovi nodi attivi e inseriscili nella Mappa
 */
void Simulation::simulate_turn(){
  if(actual_time + 1 < MAX_TIME){
    /*
    int size;
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    if(size>=2){
      simulate_turn_p();
    } else{

    }
    */
      int next_time = actual_time+1;
      printf("SIMULATE TURN: next time =%d\n",next_time);
      auto activeNodesNow = getActiveNodes();
      //create the active node list of the next turn
      for ( size_t i=0;i<activeNodesNow.size();i++){
        auto node = activeNodesNow[i];
        if(stateNextTurn(node.x, node.y) == live ){
          updateNodeState(node.x, node.y, live, next_time);
        }
      }
      
      auto nodes_spawned = calcSpawnNodes2();
      for ( size_t i=0;i<nodes_spawned.size();i++){

        updateNodeState(nodes_spawned[i].first, nodes_spawned[i].second, live, next_time);
      }
      
      //std::cout<<"simula turno numero di nodi attivi="<<activeNodesNow.size()<<"\t valore par :"<<activeNodes[next_time].size()<<"\n";

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
    }else {
      
    std::cout << "File di configurazione aperto correttamente: " << filename << std::endl;
    }
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
          //std:: cout <<"numero trovato ="<<nrs<<"\n" ;
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
          //std::  cout <<"numero trovato ="<<nrs<<"\n" ;
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
//std::cout << "Configurazione letta: " << "righe = " << config.righe << ", colonne = " << config.colonne << ", tempo = " << config.tempo << std::endl;

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

void Simulation::broadcastActiveNodes(){
  int my_rank, size;
  std::string nameFun="broadcastActiveNodes";
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  if(my_rank==0){
    auto nodi_attivi=getActiveNodes();

    std::vector<int> buffer;
    for(Nodo n:nodi_attivi){
      buffer.push_back(n.x);
      buffer.push_back(n.y);
    }

    int buffer_size = buffer.size();
    MPI_Bcast(&buffer_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(buffer.data(), buffer_size, MPI_INT, 0, MPI_COMM_WORLD);

  }else {
    auto nodi_attivi=getActiveNodes();
    int buffer_size; 
    MPI_Bcast(&buffer_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    std::vector<int> buffer(buffer_size);
    MPI_Bcast(buffer.data(), buffer_size, MPI_INT, 0, MPI_COMM_WORLD);
    for(int i=0;i<buffer_size;i=i+2){
      int target_x = buffer[i];
      int target_y = buffer[i+1];

      auto it = std::find_if(nodi_attivi.begin(), nodi_attivi.end(),
                           [target_x, target_y](const Nodo& nodo) {
                               return nodo.x == target_x && nodo.y == target_y;
                           });
      if(it == nodi_attivi.end()){
  //      cout<<"["<<nameFun<<","<<my_rank<<"] nodo inviato non trovato tra gli attivi lo inserisco tra i nodi attivi\n";
        //nodo non trovato inserisco nodo attivo
        updateNodeState(target_x,target_y,live,actual_time);
      }


    }

    }
    
    
    MPI_Barrier(MPI_COMM_WORLD);
  
}

void Simulation::calcActualNodesNextTurn(){
  int my_rank, size;
  std::string nameFun="calcActualNodesNextTurn";
  int next_time = actual_time + 1;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  std::vector<Nodo> activeNodesNow = getActiveNodes();
 // sort(activeNodesNow.begin(), activeNodesNow.end(), Simulation::customCompare);
  if(my_rank==0){
    int chunk_size = activeNodesNow.size() / (size - 1);
    for (int i = 1; i < size; ++i) {
      int start = (i - 1) * chunk_size;
      int end = (i == size - 1) ? activeNodesNow.size() - 1 : start + chunk_size - 1;
      // Inviare gli intervalli a ogni processo
      MPI_Send(&start, 1, MPI_INT, i, actual_time, MPI_COMM_WORLD);
      MPI_Send(&end, 1, MPI_INT, i, actual_time, MPI_COMM_WORLD);
      // Inviare i dati dei nodi dell'intervallo
      for (int j = start; j <= end; ++j) {
        Nodo& node = activeNodesNow[j];
        MPI_Send(&node.x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&node.y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      }
    }
    // Raccogliere gli aggiornamenti dagli altri processi
    std::vector<Point> updates; // cambiare a tipo pos e non inviare stato
    /*
    for (int i = 1; i < size; ++i) {
      int num_updates;
      MPI_Recv(&num_updates, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for (int j = 0; j < num_updates; ++j) { // deserializzare nodo ricevuto e inserirlo una sola volta in update 
        int x, y;
        Stato updated_state;
        MPI_Recv(&x, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&y, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&updated_state, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        updates.push_back(x, y, updated_state);
      }
    }
    */

    for (int i = 1; i < size; ++i) {
      int buf_size;
      MPI_Recv(&buf_size, 1, MPI_INT, i, actual_time, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      std::vector<int> buffer(buf_size);
      MPI_Recv(buffer.data(), buf_size, MPI_INT, i, actual_time, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for(int  i=0;i<buf_size;i+=2){
        Point p={buffer[i],buffer[i+1]};
        updates.push_back(p);
      }
    }

   // sort(updates.begin(), updates.end() ,Simulation::customCompareP);

    // Inviare gli aggiornamenti a tutti i processi
    
    // Applicare gli aggiornamenti localmente
    /*
    for (const auto& [x, y, updated_state] : updates) {
      cout<<"["<<nameFun<<","<<my_rank<<"] al tempo "<<actual_time<<"nodo attivo nel prossimo turno("<<x<<","<<y<<")\n";
      updateNodeState(x, y, updated_state, next_time);
    }
    */
    for(Point p:updates){
    //  cout<<"["<<nameFun<<","<<my_rank<<"] al tempo "<<actual_time<<"nodo attivo nel prossimo turno("<<p.x<<","<<p.y<<")\n";
      updateNodeState(p.x, p.y, live, next_time);
    }

  //  sort(activeNodesNow.begin(), activeNodesNow.end(), Simulation::customCompare);
  }else{
    std::vector<Nodo> activeNodesNow = getActiveNodes();
    int start,end;
    MPI_Recv(&start, 1, MPI_INT, 0, actual_time, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&end, 1, MPI_INT, 0, actual_time, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if(my_rank==1){
/*
    for (int i = 0; i <= 3; ++i) {
      Nodo node = activeNodesNow[i];
      cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"] nodo in esame con intervallo esplicito: "<< node.x<<","<<node.y<<"\n";
      }
      */
    }

   // cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"] intervalli di lavoro:"<< start<<","<<end<<"\n";
    std::vector<Point> local_updates;
    for (int i = start; i <= end; ++i) {
      Nodo node = activeNodesNow[i];
     // cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"] nodo in esame "<< node.x<<","<<node.y<<"\n";
      if (stateNextTurn(node.x, node.y) == live) {
        //cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"] nodo attivo prossimo turno "<< node.x<<","<<node.y<<"\n";
       // updateNodeState(node.x, node.y, live, next_time);
        local_updates.push_back({node.x,node.y});
      }
    }

    // Inviare gli aggiornamenti al processo principale
    std::vector<int> buffer;
    for(Point p:local_updates){
      buffer.push_back(p.x);
      buffer.push_back(p.y);
    }

    int buffer_size = buffer.size();
    MPI_Send(&buffer_size, 1, MPI_INT, 0, actual_time, MPI_COMM_WORLD);
    MPI_Send(buffer.data(), buffer_size, MPI_INT, 0, actual_time, MPI_COMM_WORLD);
    /*
    for (const auto& [x, y, updated_state] : local_updates) { //serializzare invio risultati 
      MPI_Send(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      MPI_Send(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      MPI_Send(&updated_state, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    */
  }
}

void Simulation::calcSpawnNodesP(){
  int my_rank, size;
  std::string nameFun="calcSpawnNodesP";
  int next_time = actual_time + 1;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  std::vector<Nodo> activeNodesNow = getActiveNodes();
//  std::sort(activeNodesNow.begin(), activeNodesNow.end(), Simulation::customCompare);
  if (my_rank == 0 ) {
     // cout<<"["<<nameFun<<","<<my_rank<<"] numero nodi attivi= "<<activeNodesNow.size()<<" al tempo= "<<actual_time<<"\n";

    //std::vector<std::pair<int, int>> result;
    std::map<Point,int> map;
    for (int i = 1; i < size; i++) { // val originale di i=1
      int total_elements = activeNodesNow.size();
      int start = (i==1)?0:(total_elements / 2);
      int end =(i==1)?((total_elements / 2) - 1):(total_elements-1);
      MPI_Send(&start, 1, MPI_INT, i, actual_time, MPI_COMM_WORLD);
      MPI_Send(&end, 1, MPI_INT, i, actual_time, MPI_COMM_WORLD);
      //if(actual_time>1)
       // cout<<"["<<nameFun<<","<<actual_time<<"] invio estremi("<<start<<","<<end<<") a "<<i<<"\n";

    }

    // ricezione dei dati della grande hashmap
    for(int i=1;i<size;i++){
      int buf_size;
      MPI_Recv(&buf_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      std::vector<int> buffer(buf_size);
      MPI_Recv(buffer.data(), buf_size, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      for (size_t i = 0; i < buffer.size(); i += 3) {
        Point pos = {buffer[i], buffer[i + 1]};
        int value = buffer[i + 2];
        if(map.count(pos)==1)
          map[pos]+=value;
        else 
          map[pos]=value;
        //if(i==2)
         // cout<<"["<<nameFun<<","<<actual_time <<"]Pos("<<pos.x<<","<<pos.y<<")="<<value<<"\tmap="<<map.at(pos)<<"\n";
      }
    }
    for(auto e:map){
      //std::cout<<"calcSpawnNodes2Unione["<<actual_time<<"]=("<<e.first.x<<","<<e.first.y<<")="<<e.second<<"\n";
      if(e.second==3 ){
        std::pair<int, int> sup (e.first.x,e.first.y);
          //cout<<"["<<nameFun<<","<<actual_time <<"]Pos("<<sup.first<<","<<sup.second<<")\n";
        //result.push_back(sup);
        updateNodeState(e.first.x,e.first.y, live, next_time);
      }
    }

  }else {
    int start,end;
    MPI_Recv(&start, 2, MPI_INT, 0, actual_time, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&end, 2, MPI_INT, 0, actual_time, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // if(my_rank==2)
    //  cout<<"["<<nameFun<<","<<actual_time<<"]estremi ricevuti("<<start<<","<<end<<") da "<<my_rank<<"\n";
    std::map<Point,int> map_l;
    for(int i=start;i<=end;i++){

      //if(actual_time>0)
      //cout<<"["<<nameFun<<","<<my_rank<<"] NODO ATTIVIO CONSIDERATO ("<<activeNodesNow[i].x<<","<<activeNodesNow[i].y<<") at time ="<<actual_time<<"\n";
      auto neighbours = getNeighbours(activeNodesNow[i].x,activeNodesNow[i].y);
      for(auto vicino:neighbours){

        //cout<<"["<<nameFun<<","<<my_rank<<"] NODO ATTIVIO CONSIDERATO ("<<activeNodesNow[i].x<<","<<activeNodesNow[i].y<<") at time ="<<actual_time<<" VICINO CONSIDERATO("<<(vicino.x)<<","<<(vicino.y)<<")\n";

        if(*vicino.stato == dead){
          Point tmp_pos={vicino.x,vicino.y};
          //if(my_rank==2)
          // cout<<"["<<nameFun<<","<<my_rank<<"] NODO ATTIVIO CONSIDERATO ("<<activeNodesNow[i].x<<","<<activeNodesNow[i].y<<") at time ="<<actual_time<<" VICINO CONSIDERATO("<<(vicino.x)<<","<<(vicino.y)<<")\n";
          if(map_l.count(tmp_pos)==1)
            map_l[tmp_pos]++;
          else 
            map_l[tmp_pos]=1;
        }
      }
    }
    // invia map a master
    std::vector<int> buffer;
    for (const auto& e : map_l) {
      //cout<<"["<<nameFun<<","<<my_rank<<"]("<<e.first.x<<","<<e.first.y<<")="<<e.second<<" at time ="<<actual_time<<"\n";
      if(e.second<=3){
        buffer.push_back(e.first.x);
        buffer.push_back(e.first.y);
        buffer.push_back(e.second);
      }
    }
    // Invio del numero di elementi (dimensione del buffer)
    int buffer_size = buffer.size();
    MPI_Send(&buffer_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Send(buffer.data(), buffer_size, MPI_INT, 0, 1, MPI_COMM_WORLD);

  } 

  MPI_Barrier(MPI_COMM_WORLD);
}

void Simulation::simulate_turn_inv_2(){
  int my_rank, size;
  std::string nameFun="SIMULATE_TURN_INV_2";
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  

  /*
  if( my_rank==1){
    std::cout <<"["<<nameFun<<","<<actual_time<<"] mappa attuale\n";
    printMap();
  }
  */
  calcActualNodesNextTurn();
  MPI_Barrier(MPI_COMM_WORLD);
  calcSpawnNodesP();
  MPI_Barrier(MPI_COMM_WORLD);
  advanceTime();
  MPI_Barrier(MPI_COMM_WORLD);
  broadcastActiveNodes();
  MPI_Barrier(MPI_COMM_WORLD);

  
}
