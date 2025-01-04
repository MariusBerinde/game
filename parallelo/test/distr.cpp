#include "../include/simulation.h"

using namespace std;
float tdiff(struct timeval *start,struct timeval *end){
  return (end->tv_sec-start->tv_sec) + 1e-6 * (end->tv_usec-start->tv_usec);
}

std::vector<std::pair<int,int>> Simulation::build_intervals(int nr_active_nodes){
    std::vector<std::pair<int,int>> ris;
    auto active_nodes=getActiveNodes();
  if (nr_active_nodes==1) {
    ris.push_back({0,active_nodes.size()});
  }
  if(nr_active_nodes==2){
    int size=active_nodes.size();
    ris.push_back({0,size/2-1});
    ris.push_back({size/2,size});
  }
  return ris;
}

void test_casi_banali(){

  string nameFun="TEST_CASI_BANALI";
  Simulation sim(10, 10, 10);
  sim.load_config("src/config2.txt");
  //caso 1: un solo nodo 
  vector<std::pair<int,int>> case1=sim.build_intervals(1);
  if(case1.size()==1){
    cout<<nameFun<<":Caso1: Numero di chunk corretto\n";
    auto nodi=sim.getActiveNodes();
    if(case1[0].first==0 && case1[0].second==(int)nodi.size())
      cout<<nameFun<<":Caso1: intervallo corretto\n";
    else
      cout<<nameFun<<":Caso1: intervallo errato\n";
  }else{
    cout<<nameFun<<":Caso1: Numero di chunk errato\n";

  }

  
  vector<pair<int,int>> case2=sim.build_intervals(2);
    
  if(case2.size()==2){
    cout<<nameFun<<":Caso2: Numero di chunk corretto\n";
    auto nodi=sim.getActiveNodes();
    if(case2[0].first==0 && case2[0].second==nodi.size()/2-1)
      cout<<nameFun<<":Caso2: intervallo chunk 1 corretto\n";
    else
      cout<<nameFun<<":Caso2: intervallo chunk 1 errato\n";

    if(case2[1].first==(nodi.size()/2) && case2[1].second==nodi.size())
      cout<<nameFun<<":Caso2: intervallo chunk 2 corretto\n";
    else
      cout<<nameFun<<":Caso2: intervallo chunk 2 errato\n";
  }else{
    cout<<nameFun<<":Caso2: Numero di chunk errato\n";

  }
}

int main(int argc,char *argv[]) {
		test_casi_banali();
    
  return 0;
}

