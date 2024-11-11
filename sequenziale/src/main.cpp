#include "../include/simulation.h"
#include <bits/types/struct_timeval.h>
#include <cstdio>
#include <iostream>
#include <utility>
#include <vector>
#include <sys/time.h>

float tdiff(struct timeval *start,struct timeval *end){
  return (end->tv_sec-start->tv_sec) + 1e-6 * (end->tv_usec-start->tv_usec);
}

void test_creation(){

  // Creazione di un'istanza di Simulation con 5 righe, 5 colonne e 10 unità di tempo
  Simulation sim(5, 5, 10);

  std::cout << "Test creation\n";
  // Aggiornamento di alcuni nodi al tempo 0
  sim.updateNodeState(1, 2, live, 0);
  sim.updateNodeState(2, 3, live, 0);
  sim.updateNodeState(4, 4, live, 0);

  // Stampa della mappa
  sim.printMap();


  // Avanzare il tempo della simulazione
  sim.advanceTime();

  // Aggiornare un nodo al nuovo tempo e stamparne lo stato
  sim.updateNodeState(3, 3, live, 1);
  sim.printMap();


  std::cout << "verifica getter dei vicini:";
  auto actualN = sim.getActiveNodes();
  auto actualNt = sim.getActiveNodesAtTime(0);
  std::cout << "test dimensione = "<<actualN.size()<<"\n";
  std::cout << "test dimensione con la get at time = "<<actualNt.size()<<"\n";
  for(int i=0;i<actualN.size();i++){
    std::cout<<"x="<<actualN[i].x<<"\ty="<<actualN[i].y<<"\n";
  }

  for(int i=0;i<actualNt.size();i++){
    std::cout<<"x="<<actualNt[i].x<<"\ty="<<actualN[i].y<<"\n";
  }

}

void test_get_vicini(){

  Simulation sim(6, 6, 10);

  std::cout<<"TEST GET VICINI\n" ;
  std::vector<Nodo> viciniA = sim.getNeighbours(0,0);
  if(viciniA.size()==3){
    std::cout << "Numero di vicini di A ok\n";
    bool esiti[3]={false,false,false};
    for(int i=0;i<viciniA.size();i++){
      if(viciniA.at(i).x== 0 && viciniA.at(i).y == 1)
        esiti[0] = true;
      if(viciniA.at(i).x== 1 && viciniA.at(i).y == 0)
        esiti[1] = true;

      if(viciniA.at(i).x== 1 && viciniA.at(i).y == 1)
        esiti[2] = true;
    }

    if(esiti[0])
      std::cout << "Nodo 0,1 trovato\n";
    else
      std::cout << "Nodo 0,1 non trovato\n";


    if(esiti[1])
      std::cout << "Nodo 1,0 trovato\n";
    else
      std::cout << "Nodo 1,0 non trovato\n";


    if(esiti[2])
      std::cout << "Nodo 1,1 trovato\n";
    else
      std::cout << "Nodo 1,1 non trovato\n";

  }
  else {
    std::cout << "numero di vicini sbagliato per il nodo A\n";
  }



  std::vector<Nodo> viciniB = sim.getNeighbours(0,5);
  if(viciniB.size()==3){
    std::cout << "Numero dei di B vicini ok\n";
    bool esiti[3]={false,false,false};
    for(int i=0;i<viciniB.size();i++){
      if(viciniB.at(i).x== 0 && viciniB.at(i).y == 4)
        esiti[0] = true;
      if(viciniB.at(i).x== 1 && viciniB.at(i).y == 4)
        esiti[1] = true;

      if(viciniB.at(i).x== 1 && viciniB.at(i).y == 5)
        esiti[2] = true;
    }

    if(esiti[0])
      std::cout << "Nodo 0,4 trovato\n";
    else
      std::cout << "Nodo 0,4 non trovato\n";


    if(esiti[1])
      std::cout << "Nodo 1,4 trovato\n";
    else
      std::cout << "Nodo 1,4 non trovato\n";


    if(esiti[2])
      std::cout << "Nodo 1,5 trovato\n";
    else
      std::cout << "Nodo 1,5 non trovato\n";

  }
  else {
    std::cout << "numero di vicini sbagliato per il nodo B\n";
  }



  std::vector<Nodo> viciniD = sim.getNeighbours(5,0);
  if(viciniD.size()==3){
    std::cout << "Numero dei vicini di D ok\n\n";
    bool esiti[3]={false,false,false};
    for(int i=0;i<viciniD.size();i++){
      if(viciniD.at(i).x== 4 && viciniD.at(i).y == 0)
        esiti[0] = true;
      if(viciniD.at(i).x== 4 && viciniD.at(i).y == 1)
        esiti[1] = true;

      if(viciniD.at(i).x== 5 && viciniD.at(i).y == 1)
        esiti[2] = true;
    }

    if(esiti[0])
      std::cout << "Nodo 4,0 trovato\n";
    else
      std::cout << "Nodo 4,0 non trovato\n";


    if(esiti[1])
      std::cout << "Nodo 4,1 trovato\n";
    else
      std::cout << "Nodo 4,1 non trovato\n";


    if(esiti[2])
      std::cout << "Nodo 5,1 trovato\n";
    else
      std::cout << "Nodo 5,1 non trovato\n";

  }
  else {
    std::cout << "numero di vicini sbagliato per il nodo D\n";
  }


  std::vector<Nodo> viciniF = sim.getNeighbours(5,5);
  if(viciniF.size()==3){
    std::cout << "Numero di vicini di F ok\n";
    bool esiti[3]={false,false,false};
    for(int i=0;i<viciniF.size();i++){
      if(viciniF.at(i).x== 4 && viciniF.at(i).y == 5)
        esiti[0] = true;
      if(viciniF.at(i).x== 4 && viciniF.at(i).y == 4)
        esiti[1] = true;

      if(viciniF.at(i).x== 5 && viciniF.at(i).y == 4)
        esiti[2] = true;
    }

    if(esiti[0])
      std::cout << "Nodo 4,5 trovato\n";
    else
      std::cout << "Nodo 4,5 non trovato\n";


    if(esiti[1])
      std::cout << "Nodo 4,4 trovato\n";
    else
      std::cout << "Nodo 4,4 non trovato\n";


    if(esiti[2])
      std::cout << "Nodo 5,4 trovato\n";
    else
      std::cout << "Nodo 5,4 non trovato\n";

  }

  else {
    std::cout << "numero di vicini sbagliato per il nodo F\n";
  }


  std::vector<Nodo> viciniE = sim.getNeighbours(5,3);
  if(viciniE.size()==5){
    std::cout << "Numero di vicini di E ok\n";
    bool esiti[5]={false,false,false,false,false};
    for(int i=0;i<viciniE.size();i++){
      if(viciniE.at(i).x== 5 && viciniE.at(i).y == 2)
        esiti[0] = true;
      if(viciniE.at(i).x== 4 && viciniE.at(i).y == 2)
        esiti[1] = true;

      if(viciniE.at(i).x== 4 && viciniE.at(i).y == 3)
        esiti[2] = true;

      if(viciniE.at(i).x== 4 && viciniE.at(i).y == 4)
        esiti[3] = true;

      if(viciniE.at(i).x== 5 && viciniE.at(i).y == 4)
        esiti[4] = true;
    }

    if(esiti[0])
      std::cout << "Nodo 5,2 trovato\n";
    else
      std::cout << "Nodo 5,2 non trovato\n";


    if(esiti[1])
      std::cout << "Nodo 4,2 trovato\n";
    else
      std::cout << "Nodo 4,2 non trovato\n";

    if(esiti[2])
      std::cout << "Nodo 4,3 trovato\n";
    else
      std::cout << "Nodo 4,3 non trovato\n";


    if(esiti[3])
      std::cout << "Nodo 4,4 trovato\n";
    else
      std::cout << "Nodo 4,4 non trovato\n";

    if(esiti[4])
      std::cout << "Nodo 5,4 trovato\n";
    else
      std::cout << "Nodo 5,4 non trovato\n";

  }

  else {
    std::cout << "numero di vicini sbagliato per il nodo E\n";
  }



  std::vector<Nodo> viciniC = sim.getNeighbours(3,2);
  if(viciniC.size()==8){
    std::cout << "Numero di vicini di C ok\n";
    bool esiti[8]={false,false,false,false,false,false,false,false};
    for(int i=0;i<viciniC.size();i++){
      if(viciniC.at(i).x== 2 && viciniC.at(i).y == 2)
        esiti[0] = true;

      if(viciniC.at(i).x== 2 && viciniC.at(i).y == 3)
        esiti[1] = true;

      if(viciniC.at(i).x== 3 && viciniC.at(i).y == 3)
        esiti[2] = true;

      if(viciniC.at(i).x== 4 && viciniC.at(i).y == 3)
        esiti[3] = true;

      if(viciniC.at(i).x== 4 && viciniC.at(i).y == 2)
        esiti[4] = true;

      if(viciniC.at(i).x== 4 && viciniC.at(i).y == 1)
        esiti[5] = true;

      if(viciniC.at(i).x== 3 && viciniC.at(i).y == 1)
        esiti[6] = true;

      if(viciniC.at(i).x== 2 && viciniC.at(i).y == 1)
        esiti[7] = true;
    }

    if(esiti[0])
      std::cout << "Nodo 5,2 trovato\n";
    else
      std::cout << "Nodo 5,2 non trovato\n";


    if(esiti[1])
      std::cout << "Nodo 3,3 trovato\n";
    else                  
      std::cout << "Nodo 3,3 non trovato\n";

    if(esiti[2])
      std::cout << "Nodo 4,3 trovato\n";
    else
      std::cout << "Nodo 4,3 non trovato\n";


    if(esiti[3])
      std::cout << "Nodo 4,3 trovato\n";
    else
      std::cout << "Nodo 4,3 non trovato\n";

    if(esiti[4])
      std::cout << "Nodo 4,2 trovato\n";
    else
      std::cout << "Nodo 4,2 non trovato\n";

    if(esiti[5])
      std::cout << "Nodo 4,1 trovato\n";
    else
      std::cout << "Nodo 4,1 non trovato\n";

    if(esiti[6])
      std::cout << "Nodo 3,1 trovato\n";
    else
      std::cout << "Nodo 3,1 non trovato\n";

    if(esiti[7])
      std::cout << "Nodo 2,1 trovato\n";
    else
      std::cout << "Nodo 2,1 non trovato\n";
  }

  else {
    std::cout << "numero di vicini sbagliato per il nodo C\n";
  }



  std::vector<Nodo> viciniG = sim.getNeighbours(2,0);

  if(viciniG.size()==5){
    std::cout << "Numero di vicini di G ok\n";
    bool esiti[5]={false,false,false,false,false};
    for(int i=0;i<viciniG.size();i++){
      if(viciniG.at(i).x== 1 && viciniG.at(i).y == 0)
        esiti[0] = true;
      if(viciniG.at(i).x== 2 && viciniG.at(i).y == 1)
        esiti[1] = true;

      if(viciniG.at(i).x== 2 && viciniG.at(i).y == 1)
        esiti[2] = true;

      if(viciniG.at(i).x== 3 && viciniG.at(i).y == 1)
        esiti[3] = true;

      if(viciniG.at(i).x== 3 && viciniG.at(i).y == 0)
        esiti[4] = true;
    }

    if(esiti[0])
      std::cout << "Nodo 1,0 trovato\n";
    else
      std::cout << "Nodo 1,0 non trovato\n";


    if(esiti[1])
      std::cout << "Nodo 1,1 trovato\n";
    else
      std::cout << "Nodo 1,1 non trovato\n";

    if(esiti[2])
      std::cout << "Nodo 2,1 trovato\n";
    else
      std::cout << "Nodo 2,1 non trovato\n";


    if(esiti[3])
      std::cout << "Nodo 3,1 trovato\n";
    else
      std::cout << "Nodo 3,1 non trovato\n";

    if(esiti[4])
      std::cout << "Nodo 3,0 trovato\n";
    else
      std::cout << "Nodo 3,0 non trovato\n";

  }
  else {
    std::cout << "numero di vicini sbagliato per il nodo G\n";
  }

  std::vector<Nodo> viciniH = sim.getNeighbours(2,5);

  if(viciniH.size()==5){
    std::cout << "Numero di vicini di H ok\n";
    bool esiti[5]={false,false,false,false,false};
    for(int i=0;i<viciniH.size();i++){
      if(viciniH.at(i).x== 1 && viciniH.at(i).y == 5)
        esiti[0] = true;
      if(viciniH.at(i).x== 1 && viciniH.at(i).y == 4)
        esiti[1] = true;

      if(viciniH.at(i).x== 2 && viciniH.at(i).y == 4)
        esiti[2] = true;

      if(viciniH.at(i).x== 3 && viciniH.at(i).y == 4)
        esiti[3] = true;

      if(viciniH.at(i).x== 3 && viciniH.at(i).y == 5)
        esiti[4] = true;
    }

    if(esiti[0])
      std::cout << "Nodo 1,5 trovato\n";
    else
      std::cout << "Nodo 1,5 non trovato\n";


    if(esiti[1])
      std::cout << "Nodo 1,4 trovato\n";
    else
      std::cout << "Nodo 1,4 non trovato\n";

    if(esiti[2])
      std::cout << "Nodo 2,4 trovato\n";
    else
      std::cout << "Nodo 2,4 non trovato\n";


    if(esiti[3])
      std::cout << "Nodo 3,4 trovato\n";
    else
      std::cout << "Nodo 3,4 non trovato\n";

    if(esiti[4])
      std::cout << "Nodo 3,5 trovato\n";
    else
      std::cout << "Nodo 3,5 non trovato\n";

  }
  else {
    std::cout << "numero di vicini sbagliato per il nodo H\n";
  }
}

void test_rules_next_turn(){
  Simulation sim(6, 6, 10);
  sim.updateNodeState(0, 0, live, 0); //A
  sim.updateNodeState(0, 1, live, 0); //B
  sim.updateNodeState(4, 1, live, 0); //C
  sim.updateNodeState(5, 0, live, 0); //D
  sim.updateNodeState(5, 2, live, 0); //E
  sim.updateNodeState(2, 0, live, 0); //F
  sim.updateNodeState(1, 0, live, 0); //G
  sim.updateNodeState(1, 1, live, 0); //H
  std::cout<<"TEST RULES NEXT TURN\n";

  sim.printMap();

  auto neighBoursA = sim.getNeighbours(0, 0);
  auto neighBoursB = sim.getNeighbours(0, 1);
  auto neighBoursC = sim.getNeighbours(4, 1);
  auto neighBoursD = sim.getNeighbours(5, 0);
  auto neighBoursE = sim.getNeighbours(5, 2);
  auto neighBoursF = sim.getNeighbours(2, 0);
  auto neighBoursG = sim.getNeighbours(1, 0);
  auto neighBoursH = sim.getNeighbours(1, 1);

  if(sim.stateNextTurn(0,0)==live)
    std::cout<<"Stato nodo A ok\tnumero vicini="<<neighBoursA.size()<<"\n";
  else {
    std::cout<<"Stato nodo A problema\tnumero vicini="<<neighBoursA.size()<<"\n";
  }

  if(sim.stateNextTurn(0,1)==live)
    std::cout<<"Stato nodo B ok\tnumero vicini="<<neighBoursB.size()<<"\n";
  else {
    std::cout<<"Stato nodo B problema\tnumero vicini="<<neighBoursB.size()<<"\n";
  }

  if(sim.stateNextTurn(4,1)==live)
    std::cout<<"Stato nodo C ok\tnumero vicini="<<neighBoursC.size()<<"\n";
  else {
    std::cout<<"Stato nodo C problema\tnumero vicini="<<neighBoursC.size()<<"\n";
  }
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
  if(sim.stateNextTurn(2,0)==live)
    std::cout<<"Stato nodo F ok\tnumero vicini="<<neighBoursF.size()<<"\n";
  else {
    std::cout<<"Stato nodo F problema\tnumero vicini="<<neighBoursF.size()<<"\n";
  }
  if(sim.stateNextTurn(1,0)==dead)
    std::cout<<"Stato nodo G ok\tnumero vicini="<<neighBoursG.size()<<"\n";
  else {
    std::cout<<"Stato nodo G problema\tnumero vicini="<<neighBoursG.size()<<"\n";
  }
  if(sim.stateNextTurn(1,1)==dead)
    std::cout<<"Stato nodo H ok\tnumero vicini="<<neighBoursH.size()<<"\n";
  else {
    std::cout<<"Stato nodo H problema\tnumero vicini="<<neighBoursH.size()<<"\n";
  }


}
void test_creation_spawn_nodes(){

  std::cout<<"TEST SPAWN NODES\n";

  Simulation sim(6, 6, 10);
  sim.updateNodeState(0, 0, live, 0); //A
  sim.updateNodeState(0, 1, live, 0); //B
  sim.updateNodeState(1, 0, live, 0); //G
  sim.updateNodeState(1, 1, live, 0); //H
  sim.updateNodeState(2, 0, live, 0); //F

  sim.updateNodeState(4, 1, live, 0); //C
  sim.updateNodeState(5, 0, live, 0); //D
  sim.updateNodeState(5, 2, live, 0); //E

  sim.printMap();
  auto active_nodes = sim.getActiveNodes();
 puts("nodi attivi:\n");
  for(int i=0;i<active_nodes.size();i++){
    std::cout<<"n="<<active_nodes[i].toString()<<"\n";

  }

  auto new_nodes=sim.calcSpawnNodes();
  bool ris[2]={false,false};
  if(new_nodes.size()==2){
    for(int i=0;i<new_nodes.size();i++){
      if(new_nodes[i].first == 2 && new_nodes[i].second == 1)
        ris[0]=true;
      if(new_nodes[i].first == 5 && new_nodes[i].second == 1)
        ris[1]=true;
    }
    if (ris[0]) 
      std::cout<<"Posizione 2,1 creata \n";

    if (ris[1]) 
      std::cout<<"Posizione 5,1 creata \n";

  }else{
    std::cout<<"non è stato calcoltato il numero di nodi corretto\n";
    for(int i=0;i<new_nodes.size();i++){
      printf("Nodo[%d]=%d,%d\n",i,new_nodes[i].first,new_nodes[i].second);
    }

  }


}

void test_pair(){
  std::vector<std::pair<int, int>> posizioni;
  std::pair<int, int> a (1,1);
  std::pair<int, int> b (2,2);
  std::cout<<"test pair\n a="<<a.first<<","<<a.second<<"\n b="<<b.first<<","<<b.second<<"\n";
  posizioni.push_back(a);
  posizioni.push_back(b);

  for(int i=0;i<posizioni.size();i++){
    printf("pos[%d]=%d,%d\n",i,posizioni[i].first,posizioni[i].second);
  }


}

void test_simulation(){
  std::cout<<"TEST SIMULATION NODES\n";

  Simulation sim(6, 6, 10);
  sim.updateNodeState(0, 0, live, 0); //A
  sim.updateNodeState(0, 1, live, 0); //B
  sim.updateNodeState(1, 0, live, 0); //G
  sim.updateNodeState(1, 1, live, 0); //H
  sim.updateNodeState(2, 0, live, 0); //F

  sim.updateNodeState(4, 1, live, 0); //C
  sim.updateNodeState(5, 0, live, 0); //D
  sim.updateNodeState(5, 2, live, 0); //E

  sim.printMap();
  printf("-------------------------------------------------------------------");
  sim.simulate_turn();

  sim.printMap();
  std::cout<<"nodi vicini:\n";
  auto actualN = sim.getActiveNodes();
  for(int i=0;i<actualN.size();i++){
   std::cout<<"nodo attuale = "<<actualN[i].toString()<<"\n";
  }

}

void test_read(){
  int righe_teo = 5;
  int col_teo = 5;
  int tempo_teo = 5;
  Pos A={0,0,"A"};
  Pos B={0,1,"B"};

  std::cout<<"TEST READ\n";
  Simulation sim(6, 6, 10);
  auto lettura =sim.read_file("src/config.txt");

  if (lettura.righe == righe_teo)
    std::cout<<"NUMERO RIGHE OK\n";
  else
    std::cout<<"NUMERO RIGHE ERROR\n";


  if (lettura.colonne == col_teo)
    std::cout<<"NUMERO COLONNE OK\n";
  else
    std::cout<<"NUMERO COLONNE ERROR\n";


  if (lettura.tempo == tempo_teo)
    std::cout<<"NUMERO TEMPO OK\n";
  else
    std::cout<<"NUMERO TEMPO ERROR\n";


  if(lettura.nodi.size() != 2){
    std::cout<<"NUMERO NODI ERROR\n";
  }
  else{
    auto nodi_p = lettura.nodi;
    bool esiti[2] = {false,false};
    for(size_t i=0;i<nodi_p.size();i++){
      if(nodi_p[i].nome == A.nome &&  nodi_p[i].x == A.x && nodi_p[i].y == A.y)
        esiti[0] = true;

      if(nodi_p[i].nome == B.nome &&  nodi_p[i].x == B.x && nodi_p[i].y == B.y)
        esiti[1] = true;
    }

    if(esiti[0]){
      std::cout<<"Punto A trovato\n";
    }
    else 
      std::cout<<"Punto A non trovato\n";


    if(esiti[0]){
      std::cout<<"Punto B trovato\n";
    }
    else 
      std::cout<<"Punto B non trovato\n";
  }

}

void test_config_from_file(){
  int righe_teo = 5;
  int col_teo = 5;
  int tempo_teo = 5;
  Pos A={0,0,"A"};
  Pos B={0,1,"B"};
  std::cout<<"TEST CONFIG FROM FILE\n";

  Simulation sim(5, 5, 10);
  struct timeval start,end;
  gettimeofday(&start, NULL);
  sim.load_config("src/config.txt");
  gettimeofday(&end, NULL);
  printf("tempo inizializzazione da file in  millisec %0.6f\n",tdiff(&start, &end));
  sim.printMap();

  if (sim.getMaxRows() == righe_teo)
    std::cout<<"NUMERO RIGHE OK\n";
  else
    std::cout<<"NUMERO RIGHE ERROR\n";


  if (sim.getMaxCols() == col_teo)
    std::cout<<"NUMERO COLONNE OK\n";
  else
    std::cout<<"NUMERO COLONNE ERROR\n";


  if (sim.getMaxTime() == tempo_teo)
    std::cout<<"NUMERO TEMPO OK\n";
  else
    std::cout<<"NUMERO TEMPO ERROR\n";

  auto active_nodes = sim.getActiveNodes();

  if(active_nodes.size() != 2){
    std::cout<<"NUMERO NODI ERROR\n";
  }
  else{
    bool esiti[2] = {false,false};
    for(size_t i=0;i<active_nodes.size();i++){
      if(active_nodes[i].x == A.x && active_nodes[i].y == A.y)
        esiti[0] = true;

      if(active_nodes[i].x == B.x && active_nodes[i].y == B.y)
        esiti[1] = true;
    }

    if(esiti[0]){
      std::cout<<"Punto A trovato\n";
    }
    else 
      std::cout<<"Punto A non trovato\n";


    if(esiti[0]){
      std::cout<<"Punto B trovato\n";
    }
    else 
      std::cout<<"Punto B non trovato\n";
  }


//  printf("-------------------------------------------------------------------");
  gettimeofday(&start, NULL);
  sim.simulate_turn();
  gettimeofday(&end, NULL);
  printf("tempo esecuzione di un turno in millisec %0.6f\n",tdiff(&start, &end));
  sim.printMap();

}


int main() {
  //  test_creation();
  //  test_get_vicini();
  // test_rules_next_turn();
  //test_creation_spawn_nodes();
  //test_pair();
 // test_simulation();
 // test_read();
  test_config_from_file();
  return 0;
}

