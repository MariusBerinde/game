#include "../include/simulation.h"
#include <bits/types/struct_timeval.h>
#include <cstdio>
#include <iostream>
#include <utility>
#include <vector>
#include <sys/time.h>
#include <thread>
#include <chrono>

using namespace std;
float tdiff(struct timeval *start,struct timeval *end){
  return (end->tv_sec-start->tv_sec) + 1e-6 * (end->tv_usec-start->tv_usec);
}


void test_config_1(int show_sim=0){

  Simulation sim(10, 10, 10);
/*
  sim.updateNodeState(0, 0, live, 0); //A
  sim.updateNodeState(0, 2, live, 0); //A
  sim.updateNodeState(1, 1, live, 0); //A
  sim.updateNodeState(2, 2, live, 0); //A
  sim.updateNodeState(3, 3, live, 0); //A
				      */
 sim.load_config("src/config1.txt");

  cout<<"TEST CONFIGURAZINE 1\n";
  
  if(show_sim==1) 
	  cout<<"Ho attivato la  visione della simulazione\n";

for (int turn = 0; turn < 4; ++turn) {
        // Comando ANSI per riposizionare il cursore in alto a sinistra
        cout << "\033[2J\033[H";

        // Stampa la mappa
        sim.printMap();

        // Simula il turno successivo
        sim.simulate_turn();

        // Pausa per visualizzare l'evoluzione
        std::this_thread::sleep_for(std::chrono::milliseconds(500*2));
    }
/*
  if(show_sim==1){
	  cout<<"Ho attivato la  visione della simulazione\n";
	  sim.printMap()
    for(size_t i=1;i<6;i++){
      sim.simulate_turn();
      sim.printMap();
    }

  }else{
  cout<<"Risultati test:\n";

    for(size_t i=1;i<6;i++){
      sim.simulate_turn();
      auto local_nodes=sim.getActiveNodes();
      auto time=sim.getActualTime();
      if(time==1 && local_nodes.size()==4){
	      cout<<"["<<time<<"]:Numero nodi ok\n";
      }
      else if(time==1){
        cout<<"["<<time<<"]"<<":Numero nodi aspettato=4 , numero nodi attivi trovati ="<<local_nodes.size()<<"\n";
      }

      if(time==2 && local_nodes.size()==6){
	      cout<<"["<<time<<"]:Numero nodi ok\n";
      }
      else if(time==2){
        cout<<"["<<time<<"]"<<":Numero nodi aspettato=6 , numero nodi attivi trovati ="<<local_nodes.size()<<"\n";

      }

      if(time==3 && local_nodes.size()==6){
	      cout<<"["<<time<<"]:Numero nodi ok\n";
      }
      else if(time==3){
        cout<<"["<<time<<"]"<<":Numero nodi aspettato=6 , numero nodi attivi trovati ="<<local_nodes.size()<<"\n";
      }

    }

  }
  */
}
/* showsim = 1 indica che voglio mostrare la simulaizione 
 * showsim = 0 indica che voglio nascondere la simulaizione (default 0)
 */
void test_config_2(int show_sim=0){

  Simulation sim(10, 10, 10);
/*
  sim.updateNodeState(0, 0, live, 0); //A
  sim.updateNodeState(0, 2, live, 0); //A
  sim.updateNodeState(1, 1, live, 0); //A
  sim.updateNodeState(2, 2, live, 0); //A
  sim.updateNodeState(3, 3, live, 0); //A
				      */
 sim.load_config("src/config2.txt");

  cout<<"TEST CONFIGURAZINE 2\n";
  if(show_sim==1){
	  cout<<"Ho attivato la  visione della simulazione\n";
    for(size_t i=0;i<6;i++){

      cout << "\033[2J\033[H";
      sim.printMap();
      sim.simulate_turn();
      std::this_thread::sleep_for(std::chrono::milliseconds(500*3));
    }

  }else{
  cout<<"Risultati test:\n";

    for(size_t i=1;i<6;i++){
      sim.simulate_turn();
      auto local_nodes=sim.getActiveNodes();
      auto time=sim.getActualTime();
      if(time==1 && local_nodes.size()==4){
	      cout<<"["<<time<<"]:Numero nodi ok\n";
      }
      else if(time==1){
        cout<<"["<<time<<"]"<<":Numero nodi aspettato=4 , numero nodi attivi trovati ="<<local_nodes.size()<<"\n";
      }

      if(time==2 && local_nodes.size()==6){
	      cout<<"["<<time<<"]:Numero nodi ok\n";
      }
      else if(time==2){
        cout<<"["<<time<<"]"<<":Numero nodi aspettato=6 , numero nodi attivi trovati ="<<local_nodes.size()<<"\n";

      }

      if(time==3 && local_nodes.size()==6){
	      cout<<"["<<time<<"]:Numero nodi ok\n";
      }
      else if(time==3){
        cout<<"["<<time<<"]"<<":Numero nodi aspettato=6 , numero nodi attivi trovati ="<<local_nodes.size()<<"\n";
      }

    }

  }
}

void print_after_sim(){
  int righe_teo =10,colonne_teo = 10,turni_teo = 10;
  // int turni=turni_teo-1;
  int turni=turni_teo-1;
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
    cout << "\033[2J\033[H";
   // sim.printMap();
    sim.simulate_turn();
    std::this_thread::sleep_for(std::chrono::milliseconds(500*2));
  }
  gettimeofday(&end, NULL);
  printf("[%s]velocità di esecuzione di %d turni  millisec %0.6f\n",__func__,tuni,tdiff(&start, &end));
  
  for(int i=0;i<turni;i++){
    cout << "\033[2J\033[H";
    sim.printMap(i);
    std::this_thread::sleep_for(std::chrono::milliseconds(500*2));

  }
  

}
int main(int argc,char *argv[]) {
  /*
 	if(argc > 1){
		//test_config_1((int)*argv[1]-48);
		test_config_2((int)*argv[1]-48);
	}else
		test_config_2();
   */
  print_after_sim();
  return 0;
}
