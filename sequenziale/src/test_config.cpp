#include "../include/simulation.h"
#include <bits/types/struct_timeval.h>
#include <cstdio>
#include <iostream>
#include <utility>
#include <vector>
#include <sys/time.h>
using namespace std;
float tdiff(struct timeval *start,struct timeval *end){
  return (end->tv_sec-start->tv_sec) + 1e-6 * (end->tv_usec-start->tv_usec);
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
	  sim.printMap();
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
}


int main(int argc,char *argv[]) {
 	if(argc > 1){
		test_config_2((int)*argv[1]-48);
	}else
		test_config_2();
    
  return 0;
}
