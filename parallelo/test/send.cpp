#include "../include/simulation.h"
#include <mpi.h>
#include <thread>
#include <chrono>
using namespace std;
float tdiff(struct timeval *start,struct timeval *end){
  return (end->tv_sec-start->tv_sec) + 1e-6 * (end->tv_usec-start->tv_usec);
}

// Funzione per inviare e ricevere la matrice

void Simulation::simulate_turn_p() {
    int my_rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int next_time = actual_time + 1;
    // Parte calcolo nodi attivi al prossimo turno
    // Parte del processo principale (rank 0)
    if (my_rank == 0) {
    vector<Nodo> activeNodesNow = getActiveNodes();
    // Ordinare e dividere i nodi tra i processi
    sort(activeNodesNow.begin(), activeNodesNow.end(), Simulation::customCompare);
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
      cout<<"[Master] nodo attivo nel prossimo turno("<<x<<","<<y<<")\n";
      updateNodeState(x, y, updated_state, next_time);
    }

    } else {

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
    cout<<"[simulate_turn_p,Master]:stampa mappa prima di calcolo spawn points\n";

      //std::vector<std::pair<int, int>> result;
      std::map<Point,int> map;

        std::vector<Nodo> activeNodesNow = getActiveNodes();
        // Ordinare e dividere i nodi tra i processi
        //sort(activeNodesNow.begin(), activeNodesNow.end(), Simulation::customCompare);

            std::cout<<"[calcSpawnNodes2,master]:numero nodi attivi:"<<activeNodesNow.size()<<"\n";
            std::cout<<"[calcSpawnNodes2,master]: nodi di MPI :"<<(size-1)<<"\n";
        //int chunk_size = activeNodesNow.size() / (size - 1);

    
    for (int i = 1; i < size; i++) {
      int start = (i==1)?0:3;
      int end =(i==1)?(2):activeNodesNow.size();
      MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Send(&end, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      std::cout<<"[calcSpawnNodes2,master]:i="<<i<<"(s="<<start<<","<<end<<")\n";
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
            cout<<"Master :Pos("<<pos.x<<","<<pos.y<<")="<<value<<"\tmap="<<map.at(pos)<<"\n";
        }
      }
    

      for(auto e:map){
        std::cout<<"calcSpawnNodes2,unione["<<actual_time<<"]=("<<e.first.x<<","<<e.first.y<<")="<<e.second<<"\n";
        if(e.second==3){
          std::pair<int, int> sup (e.first.x,e.first.y);
          //result.push_back(sup);
          updateNodeState(e.first.x,e.first.y, live, next_time);
        }
      }
   

    }
  else{
    //ricevono gli intervalli di lavoro e restituiscono i risultati 
    std::vector<Nodo> activeNodesNow = getActiveNodes();

    int start,end;

    MPI_Recv(&start, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&end, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    cout<<"["<<my_rank<<"](start="<<start<<",end="<<end<<")\n";
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
            cout<<"["<<my_rank<<"]("<<e.first.x<<","<<e.first.y<<")="<<e.second<<"\n";
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


void test_rules_next_turn(){

    Simulation sim(6, 6, 10);
    sim.updateNodeState(0, 0, live, 0); //A0 
    sim.updateNodeState(0, 1, live, 0); //B1
    sim.updateNodeState(4, 1, live, 0); //C2
    sim.updateNodeState(5, 0, live, 0); //D3
    sim.updateNodeState(5, 2, live, 0); //E4
    sim.updateNodeState(2, 0, live, 0); //F5
    sim.updateNodeState(1, 0, live, 0); //G6
    sim.updateNodeState(1, 1, live, 0); //H7
    std::cout<<"TEST RULES NEXT TURN\n";
    //sim.printMap();
    auto neighBoursA = sim.getNeighbours(0, 0);
    auto neighBoursB = sim.getNeighbours(0, 1);
    auto neighBoursC = sim.getNeighbours(4, 1);
    auto neighBoursD = sim.getNeighbours(5, 0);
    auto neighBoursE = sim.getNeighbours(5, 2);
    auto neighBoursF = sim.getNeighbours(1, 0);
    auto neighBoursG = sim.getNeighbours(1, 0);
    auto neighBoursH = sim.getNeighbours(1, 1);

    struct timeval start,end;
    gettimeofday(&start, NULL);
    sim.simulate_turn_p();
    gettimeofday(&end, NULL);

  std::cout << "Test creation\n";
  printf("velocità di init con parallizzazione  millisec %0.6f\n",tdiff(&start, &end));
    vector<Nodo> listaNodiAttivi=sim.getActiveNodes();
    //if(sim.stateNextTurn(0,0)==live)
    if(*listaNodiAttivi[0].stato==live)
      std::cout<<"Stato nodo A ok\tnumero vicini="<<neighBoursA.size()<<"\n";
    else {
      std::cout<<"Stato nodo A problema\tnumero vicini="<<neighBoursA.size()<<"\n";
    }

    //if(sim.stateNextTurn(0,0)==live)
    if(*listaNodiAttivi[1].stato==live)
      std::cout<<"Stato nodo B ok\tnumero vicini="<<neighBoursB.size()<<"\n";
    else {
      std::cout<<"Stato nodo B problema\tnumero vicini="<<neighBoursB.size()<<"\n";
    }

    //if(sim.stateNextTurn(0,0)==live)
    if(*listaNodiAttivi[2].stato==live)
      std::cout<<"Stato nodo C ok\tnumero vicini="<<neighBoursC.size()<<"\n";
    else {
      std::cout<<"Stato nodo C problema\tnumero vicini="<<neighBoursC.size()<<"\n";
    }
    /*
       if(sim.stateNextTurn(5,0)==dead)
       std::cout<<"Stato nodo D ok\tnumero vicini="<<neighBoursD.size()<<"\n";
       else {
       std::cout<<"Stato nodo D problema\tnumero vicini="<<neighBoursD.size()<<"\n";
       }
       if(sim.stateNextTurn(5,2)==dead)
       std::cout<<"Stato nodo E ok\tnumero vicini="<<neighBoursE.size()<<"\n";
       else {
       std::cout<<"Stato nodo E problema\tnumero vicini="<<neighBoursE.size()<<"\n";
       }
       */
    //if(sim.stateNextTurn(0,0)==live)
    if(*listaNodiAttivi[3].stato==live)
      std::cout<<"Stato nodo F ok\tnumero vicini="<<neighBoursF.size()<<"\n";
    else {
      std::cout<<"Stato nodo F problema\tnumero vicini="<<neighBoursF.size()<<"\n";
    }
    /*
       if(sim.stateNextTurn(1,0)==dead)
       std::cout<<"Stato nodo G ok\tnumero vicini="<<neighBoursG.size()<<"\n";
       else {
       std::cout<<"Stato nodo G problema\tnumero vicini="<<neighBoursG.size()<<"\n";
       }
       if(sim.stateNextTurn(1,1)==dead)
       std::cout<<"Stato nodo H ok\tnumero vicini="<<neighBoursH.size()<<"\n";
       else {
       std::cout<<"Stato nodo H problema\tnumero vicini="<<neighBoursH.size()<<"\n";
       }*/

}

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

  sim.simulate_turn_p();
  if(my_rank==0)
    sim.printMap();
  

  if(my_rank==0){
    sim.write_actual_sim("out/config2_parallelo.txt");
  }

  
}
void test_spawn_nodes_p(){
  int my_rank;
  Simulation sim(10, 10, 10);
  sim.load_config("src/config2.txt");
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  if(my_rank==0){
    sim.printMap();
  }

  sim.simulate_turn_inv();

  MPI_Barrier(MPI_COMM_WORLD);
  if(my_rank==0){
    sim.printMap();
    //sim.write_actual_sim("out/config2_parallelo.txt");
  }
  sim.simulate_turn_inv();
  if(my_rank==0){
    sim.printMap();
    //sim.write_actual_sim("out/config2_parallelo.txt");
  }
 
}

int main(int argc,char *argv[]){
  MPI_Init(&argc, &argv);
  //  test_rules_next_turn_2(); 
  //  test_multiple_rounds(); 
  test_spawn_nodes_p();
     MPI_Finalize();
  
}

