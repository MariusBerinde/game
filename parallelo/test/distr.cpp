#include "../include/simulation.h"
using namespace std;
struct Custom_range {
  int pid;
  pair<int,int> range; //including extrems
};
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


/*
 * @brief: create the ranges for each process, in the range the extreme are included 
 * @param buffer : the data that will be distribuited
 * @param np number-process, number of MPI active
 * @param nb minim number of buffer to each MPI process
 * */

vector<Custom_range> crea_range(vector<Point> *buffer,int np,int nb=3){
  vector<Custom_range> ris;
  string nameFun="CREA_RANGE ";
  if(buffer->size() > 0  ){
    if(buffer->size()<=np * nb){
      /*
      int i=0;
      for(int pid=1;pid <= np ; pid++){
        std::cout << nameFun<< "pid="<<pid<<" <"<<i<<","<< (i+nb-1)<<">\n";
        ris.push_back({pid,{i,i+nb-1}});
        i=i+nb;
      }
      */

      int pid=1;
      for(int i=0;i < buffer->size() ;i=i+nb){
        std::cout << nameFun<< "pid="<<pid<<" <"<<i<<","<< (i+nb-1)<<">\n";
        ris.push_back({pid,{i,i+nb-1}});
        
        pid++;
      }
      buffer->clear();
    }else{
      vector<Point> tmp;
      int i=0;
      cout<<nameFun<<" tmp estremi="<<i<<","<<(np * nb)<<"\n";
      //tmp.assign(buffer->begin(),buffer->begin()+np * nb);
      tmp.assign(buffer->begin(),(buffer->begin()+(np * nb)));
      //buffer->erase(buffer->begin(),buffer->begin()+np * nb);
      buffer->erase(buffer->begin(),(buffer->begin()+(np * nb)));
      ris=crea_range(&tmp,np,nb);
    }
  }
  return ris;
}

vector<Point>  init(){
  int LEN=30; //4
  vector<Point> ris;
  for(int i=0;i<LEN;i++)
      ris.push_back({i,i});

  return ris;
}

void test_2(){
  
  string nameFun="[TEST_2]:";
  vector<Point> nodi_attivi=init();

  cout<<nameFun<<"dati iniziali\n";
  for(int i=0;i<nodi_attivi.size();i++){
   std::cout << nameFun<<"nodi_attivi["<<i<<"]={"<<nodi_attivi[i].x<<","<<nodi_attivi[i].y<<"}\n"; 
  }
    vector<Point> to_compute(nodi_attivi);
    cout<<nameFun<<" caso di equa distribuzione del carico tra due nodi\n";
    Custom_range rt1={1,{0,1}};
    Custom_range rt2={2,{2,3}};
    vector<Custom_range> ris=crea_range(&to_compute,2,to_compute.size()/2);
    if(ris.size()==2){
      std::cout << "Numero di processi OK\n";
      if(to_compute.size()==0)
        std::cout << "Numero di nodi da calcolare  OK\n";
      else
        std::cout << "Numero di nodi da calcolare ERRORE:numero attuale"<<to_compute.size()<<"\n";
      for(auto e:ris){
          if(e.pid==rt1.pid){
            if((e.range.first == rt1.range.first) && (e.range.second == rt1.range.second))
              std::cout << "Range 1 :OK\n";
            else 
              std::cout << "Range 1 :valori attuale {"<<rt1.range.first<<","<< e.range.second<<"}\n";
          }

          if(e.pid==rt2.pid){
            if((e.range.first == rt2.range.first) && (e.range.second == rt2.range.second))
              std::cout << "Range 2 :OK\n";
            else 
              std::cout << "Range 2 :valori attuale {"<<rt2.range.first<<","<< e.range.second<<"}\n";
          }
        }
    }
    else
      std::cout << "Numero di processi Problema: la lunghezza attuale è="<<ris.size()<<"\n";
}
void test_4(){
  string nameFun="[TEST_4]:";
  vector<Point> nodi_attivi=init();
  cout<<nameFun<<"dati iniziali\n";
  for(int i=0;i<nodi_attivi.size();i++){
   std::cout << nameFun<<"nodi_attivi["<<i<<"]={"<<nodi_attivi[i].x<<","<<nodi_attivi[i].y<<"}\n"; 
  }
/*

    vector<Point> to_compute(nodi_attivi);
  
  
    cout<<nameFun<<" caso di equa distribuzione del carico tra due nodi\n";
    Custom_range rt1={1,{0,14}};
    Custom_range rt2={2,{15,29}};
    vector<Custom_range> ris=crea_range(&to_compute,2,to_compute.size()/2);
    if(ris.size()==2){
      std::cout << "Numero di processi OK\n";
      if(to_compute.size()==0)
        std::cout << "Numero di nodi da calcolare  OK\n";
      else
        std::cout << "Numero di nodi da calcolare ERRORE:numero attuale"<<to_compute.size()<<"\n";
      for(auto e:ris){
          if(e.pid==rt1.pid){
            if((e.range.first == rt1.range.first) && (e.range.second == rt1.range.second))
              std::cout << "Range 1 :OK\n";
            else 
              std::cout << "Range 1 :valori attuale {"<<rt1.range.first<<","<< e.range.second<<"}\n";
          }

          if(e.pid==rt2.pid){
            if((e.range.first == rt2.range.first) && (e.range.second == rt2.range.second))
              std::cout << "Range 2 :OK\n";
            else 
              std::cout << "Range 2 :valori attuale {"<<rt2.range.first<<","<< e.range.second<<"}\n";
          }
        }
    }
    else
      std::cout << "Numero di processi Problema: la lunghezza attuale è="<<ris.size()<<"\n";
 */ 
  
  
    vector<Point> to_compute_2(nodi_attivi);
    Custom_range rt1={1,{0,2}}; //ris teorico
    Custom_range rt2={2,{3,5}}  ; //ris teorico
    Custom_range rt3={3,{6,8}} ; //ris teorico
    Custom_range rt4={4,{9,11}}; //ris teorico
    cout<<nameFun<<" nodi da calcolare nel range 0 .. 29\nCalcolo prima iterazione\n";
    auto rp=crea_range(&to_compute_2,4);
    if(rp.size()==4){
      std::cout << "Numero di processi OK\n";
      if(to_compute_2.size()==18)
        std::cout << "prima iterazione Numero di nodi da calcolare  OK\n";
      else
        std::cout << "prima iterazione Numero di nodi da calcolare ERRORE:numero attuale"<<to_compute_2.size()<<"\n";
      for(auto e:rp){
          if(e.pid==rt1.pid){
            if((e.range.first == rt1.range.first) && (e.range.second == rt1.range.second))
              std::cout << "Range 1 :OK\n";
            else 
              std::cout << "Range 1 :valori attuale {"<<rt1.range.first<<","<< e.range.second<<"}\n";
          }

          if(e.pid==rt2.pid){
            if((e.range.first == rt2.range.first) && (e.range.second == rt2.range.second))
              std::cout << "Range 2 :OK\n";
            else 
              std::cout << "Range 2 :valori attuale {"<<rt2.range.first<<","<< e.range.second<<"}\n";
          }

          if(e.pid==rt3.pid){
            if((e.range.first == rt3.range.first) && (e.range.second == rt3.range.second))
              std::cout << "Range 3 :OK\n";
            else 
              std::cout << "Range 3 :valori attuale {"<<rt3.range.first<<","<< e.range.second<<"}\n";
          }


          if(e.pid==rt4.pid){
            if((e.range.first == rt4.range.first) && (e.range.second == rt4.range.second))
              std::cout << "Range 4 :OK\n";
            else 
              std::cout << "Range 4 :valori attuale {"<<rt4.range.first<<","<< e.range.second<<"}\n";
          }

        }

    cout<<nameFun<<" Seconda iterazione calcolo nel range attuale 0..17\n";
    rp=crea_range(&to_compute_2,4);
    if(rp.size()==4){
      if(to_compute_2.size()==6)
        std::cout << "seconda iterazione Numero di nodi da calcolare  OK\n";
      else
        std::cout << "seconda iterazione Numero di nodi da calcolare ERRORE:numero attuale"<<to_compute_2.size()<<"\n";
      for(auto e:rp){
        if(e.pid==rt1.pid){
          if((e.range.first == rt1.range.first) && (e.range.second == rt1.range.second))
            std::cout << "Range 1 :OK\n";
          else 
            std::cout << "Range 1 :valori attuale {"<<rt1.range.first<<","<< e.range.second<<"}\n";
        }

        if(e.pid==rt2.pid){
          if((e.range.first == rt2.range.first) && (e.range.second == rt2.range.second))
            std::cout << "Range 2 :OK\n";
          else 
            std::cout << "Range 2 :valori attuale {"<<rt2.range.first<<","<< e.range.second<<"}\n";
        }

        if(e.pid==rt3.pid){
          if((e.range.first == rt3.range.first) && (e.range.second == rt3.range.second))
            std::cout << "Range 3 :OK\n";
          else 
            std::cout << "Range 3 :valori attuale {"<<rt3.range.first<<","<< e.range.second<<"}\n";
        }


        if(e.pid==rt4.pid){
          if((e.range.first == rt4.range.first) && (e.range.second == rt4.range.second))
            std::cout << "Range 4 :OK\n";
          else 
            std::cout << "Range 4 :valori attuale {"<<rt4.range.first<<","<< e.range.second<<"}\n";
        }

      }

    rp=crea_range(&to_compute_2,4);

    cout<<nameFun<<" terza iterazione calcolo nel range attuale 0..5\n";
      if(rp.size()==2){

      if(to_compute_2.size()==0)
        std::cout << "terza iterazione Numero di nodi da calcolare  OK\n";
      else
        std::cout << "terza iterazione Numero di nodi da calcolare ERRORE:numero attuale"<<to_compute_2.size()<<"\n";
      }
      else{

      std::cout << "Numero di processi terza iterazione Problema errato: la lunghezza attuale è="<<rp.size()<<"\n";
      }

    }else {
      
      std::cout << "Numero di processi Problema errato: la lunghezza attuale è="<<rp.size()<<"\n";
    }


    }else
      std::cout << "Numero di processi Problema: la lunghezza attuale è="<<rp.size()<<"\n";
  
}

void test_rm(vector<int> *v){
  //std::cout << nameFun<<"provo a rimuovere le prime 5 posizioni\n";
  v->erase(v->begin(),v->begin()+5);

}
void uso_erase(){
  int LEN=10;
  vector<int> v(LEN);
  for(size_t i=0;i<LEN;i++){
    v[i]=i;
  }
  string nameFun="USO_ERASE: test_rm con puntatore\t";
  std::cout << nameFun<<"vettore iniziale\n";
  
  for(size_t i=0;i<LEN;i++) {
   std::cout << nameFun<<"v["<<i<<"]="<<v[i]<<"\n"; 
  }

  test_rm(&v);
  std::cout << nameFun<<"dopo rimozione\n";

  for(size_t i=0;i<v.size();i++) {
   std::cout << nameFun<<"v["<<i<<"]="<<v[i]<<"\n"; 
  }
}
int main(int argc,char *argv[]) {
//  uso_erase();

	//	test_casi_banali();
  //test_2();
  test_4();
    
  return 0;
}

