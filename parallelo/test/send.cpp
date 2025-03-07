#include "../include/simulation.h"
//#include <mpi.h>
#include <thread>
#include <chrono>
using namespace std;

struct Custom_range {
  int pid;
  pair<int,int> range; //including extrems
};


float tdiff(struct timeval *start,struct timeval *end){
  return (end->tv_sec-start->tv_sec) + 1e-6 * (end->tv_usec-start->tv_usec);
}


void Simulation::simulate_turn_inv(){
  int my_rank, size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  int next_time = actual_time + 1;
  vector<Nodo> activeNodesNow = getActiveNodes();
  sort(activeNodesNow.begin(), activeNodesNow.end(), Simulation::customCompare);
    if (my_rank == 0 ) {
    if(actual_time==2)
      cout<<"[simulate_turn_inv,Master] numero nodi attivi="<<activeNodesNow.size()<<" al tempo="<<actual_time<<"\n";

      //std::vector<std::pair<int, int>> result;
    std::map<Point,int> map;

    std::vector<Nodo> activeNodesNow = getActiveNodes();
        // Ordinare e dividere i nodi tra i processi
        //sort(activeNodesNow.begin(), activeNodesNow.end(), Simulation::customCompare);


    
    for (int i = 1; i < size; i++) {

    int total_elements = activeNodesNow.size();
      int start = (i==1)?0:(total_elements / 2);
      int end =(i==1)?((total_elements / 2) - 1):(total_elements);
      MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Send(&end, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
     // std::cout<<"[calcSpawnNodes2,master]:i="<<i<<"(s="<<start<<","<<end<<")\n";
    }
    // ricezione dei dati della grande hashmap
      for(int i=1;i<size;i++){
        int buf_size;
        MPI_Recv(&buf_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        vector<int> buffer(buf_size);
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
    
    // aggiornamento globale nodi_spawnati prossimo turno
    vector<int> buffer;
    for (const auto& e : map) {
      //cout<<"["<<my_rank<<"]("<<e.first.x<<","<<e.first.y<<")="<<e.second<<"\n";
      buffer.push_back(e.first.x);
      buffer.push_back(e.first.y);
      buffer.push_back(e.second);
    }

    int buffer_size = buffer.size();
    MPI_Bcast(&buffer_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(buffer.data(), buffer_size, MPI_INT, 0, MPI_COMM_WORLD);


    for(auto e:map){
      //std::cout<<"calcSpawnNodes2Unione["<<actual_time<<"]=("<<e.first.x<<","<<e.first.y<<")="<<e.second<<"\n";
      if(e.second==3){
        std::pair<int, int> sup (e.first.x,e.first.y);

        //result.push_back(sup);
        updateNodeState(e.first.x,e.first.y, live, next_time);
      }
    }

  }else {
    
    if(my_rank==1 && actual_time==1){
     cout<<"nodi attivi a tempo 1 per rank="<<activeNodesNow.size()<<"\n";
    }
    
    int start,end;
    MPI_Recv(&start, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&end, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //cout<<"["<<my_rank<<"](start="<<start<<",end="<<end<<")\n";
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
              vector<int> buffer;
          for (const auto& e : map_l) {
     //       cout<<"["<<my_rank<<"]("<<e.first.x<<","<<e.first.y<<")="<<e.second<<"\n";
            buffer.push_back(e.first.x);
            buffer.push_back(e.first.y);
            buffer.push_back(e.second);
          }
          // Invio del numero di elementi (dimensione del buffer)
          int buffer_size = buffer.size();
          MPI_Send(&buffer_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
          MPI_Send(buffer.data(), buffer_size, MPI_INT, 0, 1, MPI_COMM_WORLD);

    int total_map;
    MPI_Bcast(&total_map, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<int> buffer_tmp(total_map);
    MPI_Bcast(buffer_tmp.data(), total_map, MPI_INT, 0, MPI_COMM_WORLD);

    for (size_t i = 0; i < buffer.size(); i += 3) {
      Point pos = {buffer[i], buffer[i + 1]};
      int value = buffer[i + 2];
      if(value==3){
        updateNodeState(pos.x,pos.y, live, next_time);
      }
    }

  }
  MPI_Barrier(MPI_COMM_WORLD);
  if(my_rank==0){
    int chunk_size = activeNodesNow.size() / (size - 1);
    for (int i = 1; i < size; ++i) {
      int start = (i - 1) * chunk_size;
      int end = (i == size - 1) ? activeNodesNow.size() - 1 : start + chunk_size - 1;
      pair<int,int> range(start,end);
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
      cout<<"[Master] al tempo "<<actual_time<<"nodo attivo nel prossimo turno("<<x<<","<<y<<")\n";
      updateNodeState(x, y, updated_state, next_time);
    }
  }else{

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
    advanceTime();
  
}

/*
void test_rules_next_turn_2() {
    int my_rank;
 // MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Inizializzazione della simulazione
    Simulation sim(6, 6, 10);
    sim.updateNodeState(0, 0, live, 0); // A
    sim.updateNodeState(0, 1, live, 0); // B
    sim.updateNodeState(4, 1, live, 0); // C
    sim.updateNodeState(5, 0, live, 0); // D
    sim.updateNodeState(5, 2, live, 0); // E
    sim.updateNodeState(2, 0, live, 0); // F
    sim.updateNodeState(1, 0, live, 0); // G
    sim.updateNodeState(1, 1, live, 0); // H

    if (my_rank == 0) {
        std::cout << "TEST RULES NEXT TURN\n";
    }

    // Misurazione del tempo (solo processo master)
    struct timeval start, end;
    if (my_rank == 0) {
        gettimeofday(&start, NULL);
    }

    // Esegui la simulazione parallela
    sim.simulate_turn_p();

    // Misurazione del tempo (solo processo master)
    if (my_rank == 0) {
        gettimeofday(&end, NULL);
        printf("Tempo di esecuzione con parallelismo: %0.6f ms\n", tdiff(&start, &end));
    }

    // Controllo del risultato (solo processo master)
    if (my_rank == 0) {
        auto listaNodiAttivi = sim.getActiveNodes();

        // Test dei nodi
        auto check_node = [](const Nodo& nodo, const std::string& nome, size_t vicini, bool stato_atteso) {
            std::cout << "Stato nodo " << nome << (stato_atteso == (*nodo.stato == live) ? " OK" : " Problema")
                      << "\tNumero vicini=" << vicini << "\n";
        };

        auto neighBoursA = sim.getNeighbours(0, 0);
        auto neighBoursB = sim.getNeighbours(0, 1);
        auto neighBoursC = sim.getNeighbours(4, 1);
        auto neighBoursF = sim.getNeighbours(2, 0);

        check_node(listaNodiAttivi[0], "A", neighBoursA.size(), true);
        check_node(listaNodiAttivi[1], "B", neighBoursB.size(), true);
        check_node(listaNodiAttivi[2], "C", neighBoursC.size(), true);
        check_node(listaNodiAttivi[3], "F", neighBoursF.size(), true);
    }

    MPI_Finalize();
}
*/

vector<Custom_range> crea_range(const int nr_nodi_attivi,const int e_size,const int mpi_size){
  vector<Custom_range> ris;
  int last_s=0,last_e=0;
  for(int  i=1; last_e<(int) nr_nodi_attivi && i<=mpi_size  ;i++){
   if(i==1){
      last_s=0; last_e=e_size-1;
    }
    else{
      
      last_s+=e_size;
      last_e = last_s+e_size-1;
      if(last_e >= nr_nodi_attivi)
        break;
    }
      ris.push_back({i,{last_s,last_e}});
  }
  ris[ris.size()-1].range.second = nr_nodi_attivi-1;
  return ris;
}
/**
 * brief : verifica se il range r1 è uguale al range r2
*/
bool cmp_custom_range(Custom_range r1,Custom_range r2){
  return (r1.pid == r2.pid) && (r1.range.first==r2.range.first) && (r1.range.second == r2.range.second);
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
    vector<Custom_range> ranges=crea_range(activeNodesNow.size(),chunk_size,size);
    //cout<<"["<<__func__<<",0] tempo="<<actual_time<<" dimensione chunk "<<chunk_size<<" nr nodi mpi= "<< (size-1)<<" nr di ranges "<< ranges.size()<<" \n";
    for (int i=1,k=0; i <size ;  ++i,k++) {

//      cout<<"["<<__func__<<",0] tempo="<<actual_time<<" i= "<<i<< " k= " << k  <<" \n";
//      int start = (i - 1) * chunk_size;
//     int end = (i == size - 1) ? activeNodesNow.size() - 1 : start + chunk_size - 1;
      int start = ranges[k].range.first, end = ranges[k].range.second;
      // Inviare gli intervalli a ogni processo
      MPI_Send(&start, 1, MPI_INT, i, actual_time, MPI_COMM_WORLD);
      MPI_Send(&end, 1, MPI_INT, i, actual_time, MPI_COMM_WORLD);
      // Inviare i dati dei nodi dell'intervallo
     /* 
      for (int j = start; j <= end; ++j) {
        Nodo& node = activeNodesNow[j];
        MPI_Send(&node.x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&node.y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      }
      */
      
    }
    // Raccogliere gli aggiornamenti dagli altri processi
    std::vector<Point> updates; // cambiare a tipo pos e non inviare stato
    //TODO: inizio gruppo pre
    
//    for (int i = 1; i < size; ++i) {
//      int num_updates;
//      MPI_Recv(&num_updates, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//      for (int j = 0; j < num_updates; ++j) { // deserializzare nodo ricevuto e inserirlo una sola volta in update 
//        int x, y;
//        Stato updated_state;
//        MPI_Recv(&x, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//        MPI_Recv(&y, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//        MPI_Recv(&updated_state, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//        updates.push_back(x, y, updated_state);
//      }
//    }
    //todo: fine gruppo pre
    

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

    for(Point p:updates){
    //  cout<<"["<<nameFun<<","<<my_rank<<"] al tempo "<<actual_time<<"nodo attivo nel prossimo turno("<<p.x<<","<<p.y<<")\n"; // di debug
      updateNodeState(p.x, p.y, live, next_time);
    }


  //  sort(activeNodesNow.begin(), activeNodesNow.end(), Simulation::customCompare);
  }else{
    std::vector<Nodo> activeNodesNow = getActiveNodes();
    int start,end;
    MPI_Recv(&start, 1, MPI_INT, 0, actual_time, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&end, 1, MPI_INT, 0, actual_time, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   // if(my_rank==1){

//    for (int i = 0; i <= 3; ++i) {
//      Nodo node = activeNodesNow[i];
//      cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"] nodo in esame con intervallo esplicito: "<< node.x<<","<<node.y<<"\n";
//      }

    //}

    //cout<<"["<<__func__<<","<<my_rank<<","<<actual_time<<"] intervalli di lavoro:"<< start<<","<<end<<"\n";
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
    
//    for (const auto& [x, y, updated_state] : local_updates) { //serializzare invio risultati 
//      MPI_Send(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
//      MPI_Send(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
//      MPI_Send(&updated_state, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
//    }
    
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

    int chunk_size = activeNodesNow.size() / (size - 1);
    std::vector<Custom_range> ranges=crea_range(activeNodesNow.size(),chunk_size,size);
    //std::vector<std::pair<int, int>> result;
    std::map<Point,int> map;
    for (int i = 1,k=0; i < size; i++,k++) { // val originale di i=1

      int start = ranges[k].range.first, end = ranges[k].range.second;
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

}
void Simulation::simulate_turn_inv_2(){
  int my_rank, size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  
//TODO: provo a inserire l'invocazione condizionale se ci sono pochi nodi  
  //disabilitato attivazione sequenziale
  
 if( getActiveNodes().size()<TSN){
    simulate_turn();
    MPI_Barrier(MPI_COMM_WORLD);
  }else{ 
  
 // if(my_rank==0)
 //   cout<<"["<<__func__<<"] tempo="<<actual_time<<" numero di nodi attivi "<<getActiveNodes().size()<<" \n";
  calcActualNodesNextTurn();
  MPI_Barrier(MPI_COMM_WORLD);
  calcSpawnNodesP();
  MPI_Barrier(MPI_COMM_WORLD);
  advanceTime();
  MPI_Barrier(MPI_COMM_WORLD);
  broadcastActiveNodes();
//  MPI_Barrier(MPI_COMM_WORLD);
  }
  
}
void test_multiple_rounds(){
  int my_rank;
  Simulation sim(10, 10, 10);
  sim.load_config("src/config2.txt");
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  if (my_rank == 0) {
    std::cout << "TEST simulation  multiple rounds\n";
  }

  if(my_rank==0){
    sim.printMap();
  }

  sim.simulate_turn_inv_2();
  if(my_rank==0)
    sim.printMap();
  
  sim.simulate_turn_inv_2();
  if(my_rank==0)
    sim.printMap();
/*
  if(my_rank==0){
    sim.write_actual_sim("out/config2_tmp.txt");
  }

  sim.simulate_turn_inv_2();
  if(my_rank==0)
    sim.printMap();
  

  if(my_rank==0){
    sim.write_actual_sim("out/config2_parallelo.txt");
  }
*/
  
}


void test_big_sim2(){

  int my_rank, righe_teo =17,colonne_teo = 17,turni_teo = 10;
  struct timeval start,end;
  cout<<"["<<__func__<<","<<my_rank<<"]creazione di una simulazione con "<<righe_teo<<",righe\t "<< colonne_teo<<" colonne "<< " e "<<turni_teo <<"max turni\n";
  Simulation sim(righe_teo, colonne_teo, turni_teo);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  for(int i=0;i<colonne_teo;i++){
    sim.updateNodeState(i,0,live,0);
    sim.updateNodeState(i,9,live,0);
  }
  for(int i=1;i<9;i++){
    sim.updateNodeState(3,i,live,0);
    sim.updateNodeState(13,i,live,0);
  }

  if (my_rank == 0) {
  //  sim.printMap();
    gettimeofday(&start, NULL);
  }

  sim.simulate_turn_inv_2();
  //if (my_rank == 0) 
   // sim.printMap();
  sim.simulate_turn_inv_2();
  if (my_rank == 0) {
  gettimeofday(&end, NULL);
   // sim.printMap();
  printf("[%s,%d]velocità di esecuzione di 3 turni usando 2 nodi  millisec %0.6f\n",__func__,my_rank,tdiff(&start, &end));
  }
}
void test_spawn_nodes_p(){
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  Simulation sim(10, 10, 10);

  sim.load_config("src/config4.txt");
  MPI_Barrier(MPI_COMM_WORLD);
  if(my_rank==0){
  string nameFun="TEST_SPAWN_NODES_P";
    cout<<"["<<nameFun<<"] mappa originale\n";
    sim.printMap();
  }
  MPI_Barrier(MPI_COMM_WORLD);
  sim.simulate_turn_inv_2(); //crea turno 1
  
  if(my_rank==0)
    sim.printMap();

  //if(my_rank==1)
   // sim.printMap();
  
  MPI_Barrier(MPI_COMM_WORLD);
  sim.simulate_turn_inv_2(); //crea turno 2
  if(my_rank==0){
    sim.printMap();
//    sim.write_actual_sim("out/config2_parallelo_2.txt");
  }

}

void test_br(){
  int my_rank;
  string nameFun="TEST_BR";
  Simulation sim(10, 10, 10);
  sim.updateNodeState(9,9,live,0);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  if(my_rank==0){
    sim.load_config("src/config4.txt");
    cout<<"["<<nameFun<<","<<my_rank<<"] read config2 from file and send to others\n";
  }

  sim.broadcastActiveNodes();
  if(my_rank!=0){

    auto activeNodesNow=sim.getActiveNodes();
    if(activeNodesNow.size()!=6){
      cout<<"["<<nameFun<<","<<my_rank<<"] error: number of active nodes wrong\n numero nodi trovati="<<activeNodesNow.size()<<"\n";
    }else{

      cout<<"["<<nameFun<<","<<my_rank<<"] OK: number of active nodes correct\n ";
      bool tmp[5]={false,false,false,false,false};
      for(Nodo n:activeNodesNow){
        if(n.x==0 && n.y==0)
          tmp[0]=true;
        if(n.x==0 && n.y==2)
          tmp[1]=true;
        if(n.x==1 && n.y==1)
          tmp[2]=true;
        if(n.x==2 && n.y==2)
          tmp[3]=true;
        if(n.x==3 && n.y==3)
          tmp[4]=true;
      }
        if(tmp[0]){
          cout<<"["<<nameFun<<","<<my_rank<<"] Point 0,0 found\n ";
        }else{
          cout<<"["<<nameFun<<","<<my_rank<<"] Point 0,0 not found\n ";
        }

        if(tmp[1]){
          cout<<"["<<nameFun<<","<<my_rank<<"] Point 0,2 found\n ";
        }else{
          cout<<"["<<nameFun<<","<<my_rank<<"] Point 0,2 not found\n ";
        }
        if(tmp[2]){
          cout<<"["<<nameFun<<","<<my_rank<<"] Point 1,1 found\n ";
        }else{
          cout<<"["<<nameFun<<","<<my_rank<<"] Point 1,1 not found\n ";
        }
        if(tmp[3]){
          cout<<"["<<nameFun<<","<<my_rank<<"] Point 2,2 found\n ";
        }else{
          cout<<"["<<nameFun<<","<<my_rank<<"] Point 2,2 not found\n ";
        }
        if(tmp[4]){
          cout<<"["<<nameFun<<","<<my_rank<<"] Point 3,3 found\n ";
        }else{
          cout<<"["<<nameFun<<","<<my_rank<<"] Point 3,3 not found\n ";
        }



      


    }
  }
}

void debug_calcSpawnNodesP(){
  //schema eseguo calc spawn nodes faccio la broadcast e faccio avanzare il tempo e verifico la lista dei nodi attivi {}

  //carica config
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  Simulation sim(10, 10, 10);
  
  string nameFun="[DEBUG_CALCSPAWNNODESP]";
  sim.load_config("src/config2_tmp.txt");
  if(my_rank==0){
    cout<<nameFun<<":inizio\n";
    sim.printMap();
  }
  MPI_Barrier(MPI_COMM_WORLD);
  sim.calcSpawnNodesP();


}

void debug_calcActualNodesNextTurn(){

  //schema eseguo calcActualNodesNextTurnp nodes faccio la broadcast e faccio avanzare il tempo e verifico la lista dei nodi attivi 

  //carica config
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  Simulation sim(10, 10, 10);
  
  sim.load_config("src/config3.txt");
  string nameFun="DEBUG_CALCSPAWNNODESP";
  MPI_Barrier(MPI_COMM_WORLD);

  if(my_rank==0){
    int actual_time=sim.getActualTime();
    cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"] mappa originale\n";
    sim.printMap();
  }

  sim.calcActualNodesNextTurn(); // prima esecuzione
  MPI_Barrier(MPI_COMM_WORLD);
  sim.advanceTime();
  MPI_Barrier(MPI_COMM_WORLD);
  sim.broadcastActiveNodes();
  MPI_Barrier(MPI_COMM_WORLD);
  if(my_rank==0){
    int actual_time=sim.getActualTime();
    cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"]  mappa dopo la prima attivazione di calcNode\n";
    sim.printMap();
  }


  sim.calcActualNodesNextTurn(); //simulo la seconda volta
  MPI_Barrier(MPI_COMM_WORLD);
  sim.advanceTime();
  MPI_Barrier(MPI_COMM_WORLD);
  sim.broadcastActiveNodes();
  MPI_Barrier(MPI_COMM_WORLD);
  
  if(my_rank==0){
    int actual_time=sim.getActualTime();
    cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"]  mappa dopo la seconda attivazione di calcNode\n";
    sim.printMap();
  }

  auto nodiAttiviMaster=sim.getActiveNodes();
  auto nodiAttiviP1=sim.getActiveNodes();
  auto nodiAttiviP2=sim.getActiveNodes();
/*
  
  if(my_rank==0){
    int actual_time=sim.getActualTime();
    
    cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"]  numero nodi attivi:"<< nodiAttiviMaster.size()<<"\n";
   // sim.printMap();
    
    for(Nodo n:nodiAttiviP1){
      cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"] = "<<n.x<<","<< n.y<<"\n";
    }
  }
  
  
  if(my_rank==1){
    int actual_time=sim.getActualTime();
    cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"]  numero nodi attivi:"<< nodiAttiviP1.size()<<"\n";
      //cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"] stampo  nodi attivi \n";
    
    for(Nodo n:nodiAttiviP1){
      cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"] = "<<n.x<<","<< n.y<<"\n";
    }
    
  //  cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"] stampo  mappa attivi \n";
  //  sim.printMap();
  
  }
 */ 
  /*
 if(my_rank==2){
    int actual_time=sim.getActualTime();
    cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"]  numero nodi attivi:"<< nodiAttiviP2.size()<<"\n";
    cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"] stampo  nodi attivi \n";
    for(Nodo n:nodiAttiviP2){
      cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"] = "<<n.x<<","<< n.y<<"\n";
    }
    
    cout<<"["<<nameFun<<","<<my_rank<<","<<actual_time<<"] stampo  mappa attivi \n";
    sim.printMap();
  }
  */

}
void simula_bordo(){

  int my_rank,righe_teo =100,colonne_teo = 100,turni_teo = 1000;
  cout<<"["<<__func__<<"]creazione di una simulazione con "<<righe_teo<<",righe\t "<< colonne_teo<<" colonne "<< " e "<<turni_teo <<" max turni\n";
  Simulation sim(righe_teo, colonne_teo, turni_teo);
  for(int i=0;i<colonne_teo;i++){
    sim.updateNodeState(i,0,live,0);
    sim.updateNodeState(i,colonne_teo-1,live,0);
  }
  
  for(int i=1;i<righe_teo;i++){
    sim.updateNodeState(0,i,live,0);
    sim.updateNodeState(righe_teo-1,i,live,0);
  }
  

  struct timeval start,end;

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  if(my_rank==0)
    gettimeofday(&start, NULL);
  for(int i=0;i<turni_teo-1;i++){
//    if(my_rank==0)
//      sim.printMap();
    sim.simulate_turn_inv_2();
  }
  if(my_rank==0){
    gettimeofday(&end, NULL);
    printf("[%s]velocit� di esecuzione di %d turni  millisec %0.6f\n",__func__,turni_teo,tdiff(&start, &end));
  }
}
void simula_croce(){

  int my_rank=-1,righe_teo =100,colonne_teo = 100,turni_teo = 1000,nr_proc=-1;
  //int my_rank=-1,righe_teo =10,colonne_teo = 10,turni_teo = 10,nr_proc=-1;
  cout<<"["<<__func__<<"]creazione di una simulazione con "<<righe_teo<<",righe\t "<< colonne_teo<<" colonne "<< " e "<<turni_teo <<" max turni\n";
  Simulation sim(righe_teo, colonne_teo, turni_teo);
  for(int i=0;i<colonne_teo;i++){
    sim.updateNodeState(i,0,live,0);
    sim.updateNodeState(i,4,live,0);
    sim.updateNodeState(i,9,live,0);
    sim.updateNodeState(i,14,live,0);
    sim.updateNodeState(i,colonne_teo-1,live,0);
  }
  
  for(int i=1;i<righe_teo;i++){
    sim.updateNodeState(0,i,live,0);
    sim.updateNodeState(4,i,live,0);
    sim.updateNodeState(9,i,live,0);
    sim.updateNodeState(14,i,live,0);
    sim.updateNodeState(righe_teo-1,i,live,0);
  }
  

  struct timeval start,end;
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nr_proc);

  gettimeofday(&start, NULL);
  for(int i=0;i<turni_teo-1;i++){
/*
    if(my_rank==0)
      sim.printMap();
 */   
    sim.simulate_turn_inv_2();
  }
  if(my_rank==0){
    gettimeofday(&end, NULL);
    printf("[%s]velocità di esecuzione di 2 turni  millisec %0.6f con %d processi\n",__func__,tdiff(&start, &end),nr_proc);
  }
}
int main(int argc,char *argv[]){
  MPI_Init(&argc, &argv);
//    test_rules_next_turn_2(); 
//    test_multiple_rounds(); 
//    test_spawn_nodes_p();
//    test_br();
//    debug_calcSpawnNodesP();
//    debug_calcActualNodesNextTurn();
//    test_big_sim2();
      simula_bordo();
//    simula_croce();
     MPI_Finalize();
  
}

