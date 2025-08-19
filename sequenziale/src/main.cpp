#include "../include/simulation.h"
#include <chrono>
#include <thread>
using namespace std;

/** restituisce la differenza tra i tempi di esecuzione con precisione di 10e^-6*/
float tdiff(struct timeval *start,struct timeval *end){
  return (end->tv_sec-start->tv_sec) + 1e-6 * (end->tv_usec-start->tv_usec);
}

/**
 * funzione usata assicurarsi che la mappa da gioco è creata correttamente  
 */
void test_creation(){

  // Creazione di un'istanza di Simulation con 5 righe, 5 colonne e 10 unità di tempo
  size_t rows=40,lines=40;
  struct timeval start,end;
  gettimeofday(&start, NULL);
  Simulation sim(rows, lines, 2);
  gettimeofday(&end, NULL);

  /*
 // struct timeval start,end;
  gettimeofday(&start, NULL);
  Simulation sim(rows, lines, 2);
  gettimeofday(&end, NULL);
*/

  std::cout << "Test creation\n";
  printf("velocità di init senza parallizzazione  millisec %0.6f\n",tdiff(&start, &end));
  std::cout << "Test creation\n";
  printf("velocità di init senza parallizzazione  millisec %0.6f\n",tdiff(&start, &end));
  // Aggiornamento di alcuni nodi al tempo 0
  for ( size_t i=0;i<rows;i++){
    for (size_t j=0; j<lines; j++) {
        sim.updateNodeState(i, j, live, 0);
    }

  }

  // Stampa della mappa
  sim.printMap();

/*
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
  for ( size_t i=0;i<actualN.size();i++){
    std::cout<<"x="<<actualN[i].x<<"\ty="<<actualN[i].y<<"\n";
  }

  for ( size_t i=0;i<actualNt.size();i++){
    std::cout<<"x="<<actualNt[i].x<<"\ty="<<actualN[i].y<<"\n";
  }
*/
}

/*
 * funzione usata per testare funzionamento di sim.getNeighboursgetNeighbours con diverse configurazioni 
 */
void test_get_vicini(){

  Simulation sim(6, 6, 10);

  std::cout<<"TEST GET VICINI\n" ;
  std::vector<Nodo> viciniA = sim.getNeighbours(0,0);
  if(viciniA.size()==3){
    std::cout << "Numero di vicini di A ok\n";
    bool esiti[3]={false,false,false};
    for ( size_t i=0;i<viciniA.size();i++){
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
    for ( size_t i=0;i<viciniB.size();i++){
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
    for ( size_t i=0;i<viciniD.size();i++){
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
    for ( size_t i=0;i<viciniF.size();i++){
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
    for ( size_t i=0;i<viciniE.size();i++){
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
    for ( size_t i=0;i<viciniC.size();i++){
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
    for ( size_t i=0;i<viciniG.size();i++){
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
    for ( size_t i=0;i<viciniH.size();i++){
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


/**
 * funzione usata  per testare stateNextTurn 
 */
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

/**
 * funzione usata per verificare che la funzione usata per lo spawn dei nodi funzioni correttamente.
 */
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
  for ( size_t i=0;i<active_nodes.size();i++){
    std::cout<<"n="<<active_nodes[i].toString()<<"\n";

  }

  auto new_nodes=sim.calcSpawnNodes();
  bool ris[2]={false,false};
  if(new_nodes.size()==2){
    for ( size_t i=0;i<new_nodes.size();i++){
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
    for ( size_t i=0;i<new_nodes.size();i++){
      printf("Nodo[%ld]=%d,%d\n",i,new_nodes[i].first,new_nodes[i].second);
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

  for ( size_t i=0;i<posizioni.size();i++){
    printf("pos[%ld]=%d,%d\n",i,posizioni[i].first,posizioni[i].second);
  }


}

/**
 * funzione in cui si utilizza la funzine simulate turn per la prima volta
 */
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
  for ( size_t i=0;i<actualN.size();i++){
   std::cout<<"nodo attuale = "<<actualN[i].toString()<<"\n";
  }

}

/**
 * funzione di test della lettura del file di configurazione
 */
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

/**
 * funzione di test della lettura del file di configurazione e integrazione con simulazione turno.
 */
void test_config_from_file(){
  int righe_teo = 5;
  int col_teo = 5;
  int tempo_teo = 5;
  Pos A={0,0,"A"};
  Pos B={0,1,"B"};
  std::cout<<"TEST CONFIG FROM FILE\n";

  Simulation sim(5, 5, 10);
  struct timeval start,end;
  //gettimeofday(&start, NULL);
  sim.load_config("src/config.txt");
 // gettimeofday(&end, NULL);
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

/*
 * simualzione basilare con 3 nodi attivi
 */
void seq(){ 
  int my_rank = 0;
  Simulation sim(6, 6, 2);
  sim.updateNodeState(0, 0, live, 0); // A
  sim.updateNodeState(0, 1, live, 0); // B
  sim.updateNodeState(1, 0, live, 0); // G
  auto nodi_attivi = sim.getActiveNodes();
  printf("Processo[%d](%d)\tnumero di nodi attivi %ld\n",my_rank,sim.getActualTime(),nodi_attivi.size());
  sim.printMap();
  sim.simulate_turn();
  nodi_attivi = sim.getActiveNodes();
  printf("Processo[%d](%d)\tnumero di nodi attivi %ld\n",my_rank,sim.getActualTime(),nodi_attivi.size());
  sim.printMap();
}

/**
 * test della fun che calcola la distanza di eulero tra due punti .
 */
void test_eu_dist(){
  Simulation sim(6, 6, 2);
  sim.updateNodeState(0, 0, live, 0); // A
  sim.updateNodeState(0, 1, live, 0); // B
  sim.updateNodeState(1, 0, live, 0); // G
  sim.updateNodeState(4, 0, live, 0); 
  auto nodi_attivi = sim.getActiveNodes();
  std::cout<<"TEST DISTANZA EUCLIDEA\n";
  sim.printMap();
  Nodo b = {5, 5, NULL};
  for (size_t i = 0; i < nodi_attivi.size(); i++) {
    auto nodo_attuale = nodi_attivi[i];
    printf("differenza tra (%d,%d) e (%d,%d) = %lf\n",nodo_attuale.x,nodo_attuale.y,b.x,b.y,sim.eu_distance_node(nodo_attuale,b));
  }

}

/**
 * verfica di correttezza della distanza di MANHATTAN tra due punti.
 */
void test_mh_dist(){
  Simulation sim(6, 6, 2);
  sim.updateNodeState(0, 0, live, 0); // A
  sim.updateNodeState(0, 1, live, 0); // B
  sim.updateNodeState(1, 0, live, 0); // G
  sim.updateNodeState(4, 0, live, 0); 
  auto nodi_attivi = sim.getActiveNodes();

  std::cout<<"TEST DISTANZA DI MANHATTAN \n";
  sim.printMap();
  Nodo b = {5, 5, NULL};
  for (size_t i = 0; i < nodi_attivi.size(); i++) {
    auto nodo_attuale = nodi_attivi[i];
    printf("differenza tra (%d,%d) e (%d,%d) = %d\n",nodo_attuale.x,nodo_attuale.y,b.x,b.y,sim.mh_distance_node(nodo_attuale,b));
  }

}
bool customCompare_l(Nodo a, Nodo b) {
       return (a.x<b.x) && (a.y<b.y);
     }

void test_order_node_vector(){
  Nodo b = {5, 5, NULL};
  Nodo a = {0, 0, NULL};
  Nodo c = {1, 3, NULL};
  Nodo d = {4, 2, NULL};
  std::vector<Nodo> l={a,b,c,d};
  std::cout << "TEST ORDER VECTOR\n vettore originale\n:";
  for(Nodo n:l){
    printf("(%d,%d)\t",n.x,n.y);
  }
  printf("\n vettore ordinato");

  sort(l.begin(),l.end(),customCompare_l);

  for(Nodo n:l){
    printf("(%d,%d)\t",n.x,n.y);
  }
  printf("\n");

}
vector<pair<int,int>> gen_random_pos(int nr,int max){
  vector<pair<int,int>> ris;

    int min = 0;

    // Initialize a random number generator
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(min, max);

    // Generate random number in the range [min, max]
    //int x = distrib(gen);
  for(int i=0;i<nr;i++){

    int x = distrib(gen);
    int y = distrib(gen);
    pair<int,int> tmp={x,y};
    if(i==0){
      ris.push_back(tmp);
    }
    else{
      pair<int,int> tmp2= {x,y};
      auto it1 = find(ris.begin(),ris.end(),tmp2);
      bool is_in = (it1!= ris.end());
      while(is_in){
        x = distrib(gen);
        y = distrib(gen);
        tmp2={x,y};
      }
      ris.push_back(tmp2);
    }

  }
  
  return ris;
}


/**
 * crea una simulazione con punti a caso usando come parametri la dimesione della mappa e il numero di nodi
 */
Simulation make_random_sim(int righe,int colonne,int turni,int numero_nodi){
  Simulation sim(righe,colonne,turni);
  auto position_list = gen_random_pos(numero_nodi,righe);
  for(auto p:position_list){
    sim.updateNodeState(p.first, p.second, live, 0);
  }
  return sim;
}

/**
 * verifica della correttezza di make_random_sim
 */
void test_random_sim(){
  int righe_teo =10,colonne_teo = 10,turni_teo = 5,max_nodi_teo = 13;
  Simulation sim = make_random_sim(righe_teo,colonne_teo,turni_teo,max_nodi_teo);
  int righe_gen = sim.getMaxRows();
  int colonne_gen = sim.getMaxCols();
  int turni_gen = sim.getMaxTime();
  auto nodi_attivi = sim.getActiveNodes();
  if( righe_teo == righe_gen )
    std::cout << "Numero righe ok\n";
  else 
    std::cout << "Numero righe errato\n";


  if( colonne_teo == colonne_gen )
    std::cout << "Numero colonne ok\n";
  else 
    std::cout << "Numero colonne errato\n";

  if( turni_teo == turni_gen )
        std::cout << "Numero turni ok\n";
    else 
          std::cout << "Numero turni errato\n";

  if( (int)nodi_attivi.size() == max_nodi_teo ){
        std::cout << "Numero nodi attivi ok\n nodi attivi generati:\n";
        for(Nodo n:nodi_attivi){
          printf("Nodo(%d,%d)\n",n.x,n.y);
        }
  }
    else {

        std::cout << "Numero nodi attivi errati\n nodi attivi generati:\n";
        for(Nodo n:nodi_attivi){
          printf("Nodo(%d,%d)\n",n.x,n.y);
        }
    }

  

}


/**
 * simulaizone con una matrice 25x25 e 20 turni simulati 
 */
void test_big_sim(){
  int righe_teo =25,colonne_teo = 25,turni_teo = 10,max_nodi_teo = 20;
  cout<<"["<<__func__<<"]creazione di una simulazione con "<<righe_teo<<",righe\t "<< colonne_teo<<" colonne "<< max_nodi_teo<<" e "<<turni_teo <<"max turni\n";
  //Simulation sim = make_random_sim(righe_teo,colonne_teo,turni_teo,max_nodi_teo);

  Simulation sim(righe_teo, colonne_teo, turni_teo);
  cout<<"["<<__func__<<"] creazione nodi attivi random ...\n";
  auto position_list = gen_random_pos(max_nodi_teo,righe_teo);
  for(auto &[x,y]:position_list){
    sim.updateNodeState(x,y, live, 0);
  }
  sim.printMap();
  sim.simulate_turn();
  sim.printMap();
}

/**
 * simulaizone con una matrice 17 x 17 e 10  turni simulati 
 */
void test_big_sim2(){
  int righe_teo =17,colonne_teo = 17,turni_teo = 10;
  cout<<"["<<__func__<<"]creazione di una simulazione con "<<righe_teo<<",righe\t "<< colonne_teo<<" colonne "<< " e "<<turni_teo <<"max turni\n";
  Simulation sim(righe_teo, colonne_teo, turni_teo);
  for(int i=0;i<colonne_teo;i++){
    sim.updateNodeState(i,0,live,0);
    sim.updateNodeState(i,9,live,0);
  }
  for(int i=1;i<9;i++){
    sim.updateNodeState(3,i,live,0);
    sim.updateNodeState(13,i,live,0);
  }

  struct timeval start,end;

  sim.printMap();
  gettimeofday(&start, NULL);
  sim.simulate_turn();
  //sim.printMap();
  sim.simulate_turn();
  gettimeofday(&end, NULL);
  sim.printMap();
  printf("[%s]velocità di esecuzione di 3 turni  millisec %0.6f\n",__func__,tdiff(&start, &end));
}

/**
 * funzione usata per scrivere le prestazioni di una simulazione sul file filename, sim è la simualzione, var è il tempo.
 */
void write_performace_sim(const std::string& filename,Simulation& sim,float var,int MPI_SIZE=0){

  std::ofstream out_file;
  //std::cout << "Apertura file "<<filename<<"\n";
  std::string intestazione="***Simulazione Game of life in data:"+std::string(__DATE__)+" e ora "+std::string(__TIME__)+" ***\n";
  std::string map_size="dimensione mappa: " + to_string(sim.getMaxRows()) + " x " + to_string( sim.getMaxCols())+"\n";
  std::string nr_turn="numero di turni:"+to_string(sim.getActualTime())+"\n";
  std::string start_active_time="numero di nodi inizial attivi:"+to_string(sim.getActiveNodesAtTime(0).size())+"\n";
  std::string performace="tempo di esecuzione:"+to_string(var)+"\n";
  std::string nr_mpi="nr nodi mpi:"+to_string(MPI_SIZE)+"\n";
  //std::string file= intestazione+map_size+nr_turn+start_active_time+performace+nr_mpi;
  std::string file= intestazione+map_size+nr_turn+start_active_time+nr_mpi;
  std::cout << "Debug "<<__func__<<"valore da inserire nel file\n"<<file;
  
  out_file.open(filename);
  //std::cout<<tag<< "Apertura file "<<filename<<"\n";
  //std::string intestazione="***Simulazione Game of life in ***\n";

  out_file<<intestazione<<map_size<<nr_turn<<start_active_time<<performace<<nr_mpi;
  //out_file.close();
  
  //std::cout<<tag
}

/**
 * simulazione in cui la figura iniziale è il bordo della scacchiera.
 */
void simula_bordo(){

  //int righe_teo =40,colonne_teo = 40,turni_teo = 100;
  int righe_teo =40,colonne_teo = 40,turni_teo = 10;
  //int righe_teo =20,colonne_teo = 20,turni_teo = 10;
  int turni=turni_teo;
  cout<<"["<<__func__<<"]creazione di una simulazione con "<<righe_teo<<",righe\t "<< colonne_teo<<" colonne "<< " e "<<turni_teo <<"max turni\n";
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

  gettimeofday(&start, NULL);
  for(int i=0;i<turni;i++){
   // sim.printMap();
    sim.simulate_turn();
  }
  gettimeofday(&end, NULL);
  printf("[%s]velocità di esecuzione di 1000 turni  millisec %0.6f\n",__func__,tdiff(&start, &end));
  write_performace_sim("out/prestazioni.txt",sim,tdiff(&start, &end));

  
  for(int i=0;i<turni;i++){
    cout << "\032[2J\033[H";
    sim.printMap(i);
    std::this_thread::sleep_for(std::chrono::milliseconds(500*2));
  }
  
}
/**
 * simulazionein cui la figura iniziale è  una croce.
 */
void simula_croce(){

  int righe_teo =40,colonne_teo = 40,turni_teo = 100;
  //int righe_teo =20,colonne_teo = 20,turni_teo = 10;//old_turni_teo=1000;

  int turni=turni_teo;
  cout<<"["<<__func__<<"]creazione di una simulazione con "<<righe_teo<<",righe\t "<< colonne_teo<<" colonne "<< " e "<<turni_teo <<"max turni\n";
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

  gettimeofday(&start, NULL);
  for(int i=0;i<turni;i++){
   // sim.printMap();
    sim.simulate_turn();
  }
  gettimeofday(&end, NULL);

  /*
  for(int i=0;i<turni;i++){
    cout << "\033[2J\033[H";
    sim.printMap(i);
    std::this_thread::sleep_for(std::chrono::milliseconds(500*2));
  }
  */

  printf("[%s]velocità di esecuzione di 10 turni  millisec %0.6f\n",__func__,tdiff(&start, &end));

  write_performace_sim("out/prestazioni_croce.txt",sim,tdiff(&start, &end));
}
/**
* funzione dove simulo una scacchiera per 10 turni in cui la mappa è un quadrato 7x7
*/
void simula_scacchiara(){


  //int righe_teo =30,colonne_teo = 30,turni_teo = 45;
  int righe_teo =7,colonne_teo = 7,turni_teo = 10;
  int turni=turni_teo;
  cout<<"["<<__func__<<"]creazione di una simulazione con "<<righe_teo<<",righe\t "<< colonne_teo<<" colonne "<< " e "<<turni_teo <<"max turni\n";
  Simulation sim(righe_teo, colonne_teo, turni_teo);
  for(int i=0;i<colonne_teo;i++){
    for(int j=0;j<righe_teo;j++){
      if((j%2==0) ){
        sim.updateNodeState(j,i,live,0);
      }
    }
  }
  
  

  struct timeval start,end;

  gettimeofday(&start, NULL);
  for(int i=0;i<turni;i++){
   // sim.printMap();
    sim.simulate_turn();
  }
  gettimeofday(&end, NULL);
  printf("[%s]velocità di esecuzione di 1000 turni  millisec %0.6f\n",__func__,tdiff(&start, &end));
  //write_performace_sim("out/prestazioni.txt",sim,tdiff(&start, &end));

  for(int i=0;i<turni;i++){
    //cout << "\033[2J\033[H";
    sim.printMap(i);
    //std::this_thread::sleep_for(std::chrono::milliseconds(500*3));
  }
}
int main() {
//  test_creation();
//  test_get_vicini();
//  test_rules_next_turn();
//  test_creation_spawn_nodes();
//  test_pair();
//  test_simulation();
//  test_read();
//  test_config_from_file();
//  seq();
//  test_eu_dist();
//  test_mh_dist();
//  test_order_node_vector();
//  test_gen_random_pos();
//  test_random_sim();
//  test_big_sim();
//  test_big_sim2();
  simula_bordo();
//  simula_croce();
//  simula_scacchiara();
  return 0;
}

