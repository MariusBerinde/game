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
vector<Custom_range> crea_range2(const int nr_nodi_attivi,const int e_size,const int mpi_size){
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

void test_1(){
  vector<int> vals;
  const int SIZE=100;
  for(int i=0;i<SIZE;vals.push_back(i),i++);
  vector<Custom_range> r1_teo;
  vector<Custom_range> r2_teo;
  vector<Custom_range> r3_teo;
  r1_teo.push_back({1,{0,9}});
  r1_teo.push_back({2,{10,19}});
  r1_teo.push_back({3,{20,29}});
  r1_teo.push_back({4,{30,99}});
  vector<Custom_range> r1_es=crea_range2(SIZE,10,4);
  if(r1_es.size()==r1_teo.size()){
    if(cmp_custom_range(r1_es[0],r1_teo[0]))
      cout<<"["<<__func__<<"] Test 1: range 1 ok\n";
    else 
      cout<<"["<<__func__<<"] Test 1: range 1 errato , valori ritornati{"<<r1_es[0].pid<<",{"<< r1_es[0].range.first<<","<<r1_es[0].range.second<<"}}\tvalori teorici{"<< r1_teo[0].pid<<",{"<< r1_teo[0].range.first<<","<<r1_teo[0].range.second<<"}}\n";

    if(cmp_custom_range(r1_es[1],r1_teo[1]))
      cout<<"["<<__func__<<"] Test 1: range 2 ok\n";
    else 
      cout<<"["<<__func__<<"] Test 1: range 2 errato , valori ritornati{"<<r1_es[1].pid<<",{"<< r1_es[1].range.first<<","<<r1_es[1].range.second<<"}}\tvalori teorici{"<< r1_teo[1].pid<<",{"<< r1_teo[1].range.first<<","<<r1_teo[1].range.second<<"}}\n";

    if(cmp_custom_range(r1_es[2],r1_teo[2]))
      cout<<"["<<__func__<<"] Test 1: range 3 ok\n";
    else 
      cout<<"["<<__func__<<"] Test 1: range 3 errato , valori ritornati{"<<r1_es[2].pid<<",{"<< r1_es[2].range.first<<","<<r1_es[2].range.second<<"}}\tvalori teorici{"<< r1_teo[2].pid<<",{"<< r1_teo[2].range.first<<","<<r1_teo[2].range.second<<"}}\n";

    if(cmp_custom_range(r1_es[3],r1_teo[3]))
      cout<<"["<<__func__<<"] Test 1: range 4 ok\n";
    else 
      cout<<"["<<__func__<<"] Test 1: range 4 errato , valori ritornati{"<<r1_es[3].pid<<",{"<< r1_es[3].range.first<<","<<r1_es[3].range.second<<"}}\tvalori teorici{"<< r1_teo[3].pid<<",{"<< r1_teo[3].range.first<<","<<r1_teo[3].range.second<<"}}\n";

  }else{
    cout<<"["<<__func__<<"] Test 1: numero di casi errato numero di casi trovavi:"<< r1_es.size()<<"\n";
  }

  vector<Custom_range> r2_es=crea_range2(SIZE,25,4);
  r2_teo.push_back({1,{0,24}});
  r2_teo.push_back({2,{25,49}});
  r2_teo.push_back({3,{50,74}});
  r2_teo.push_back({4,{75,99}});

  if(r2_es.size()==r2_teo.size()){
    if(cmp_custom_range(r2_es[0],r2_teo[0]))
      cout<<"["<<__func__<<"] Test 2: range 1 ok\n";
    else 
      cout<<"["<<__func__<<"] Test 2: range 1 errato , valori ritornati{"<<r2_es[0].pid<<",{"<< r2_es[0].range.first<<","<<r2_es[0].range.second<<"}}\tvalori teorici{"<< r2_teo[0].pid<<",{"<< r2_teo[0].range.first<<","<<r2_teo[0].range.second<<"}}\n";

    if(cmp_custom_range(r2_es[1],r2_teo[1]))
      cout<<"["<<__func__<<"] Test 2: range 2 ok\n";
    else 
      cout<<"["<<__func__<<"] Test 2: range 2 errato , valori ritornati{"<<r2_es[1].pid<<",{"<< r2_es[1].range.first<<","<<r2_es[1].range.second<<"}}\tvalori teorici{"<< r2_teo[1].pid<<",{"<< r2_teo[1].range.first<<","<<r2_teo[1].range.second<<"}}\n";

    if(cmp_custom_range(r2_es[2],r2_teo[2]))
      cout<<"["<<__func__<<"] Test 2: range 3 ok\n";
    else 
      cout<<"["<<__func__<<"] Test 2: range 3 errato , valori ritornati{"<<r2_es[2].pid<<",{"<< r2_es[2].range.first<<","<<r2_es[2].range.second<<"}}\tvalori teorici{"<< r2_teo[2].pid<<",{"<< r2_teo[2].range.first<<","<<r2_teo[2].range.second<<"}}\n";

    if(cmp_custom_range(r2_es[3],r2_teo[3]))
      cout<<"["<<__func__<<"] Test 2: range 4 ok\n";
    else 
      cout<<"["<<__func__<<"] Test 2: range 4 errato , valori ritornati{"<<r2_es[3].pid<<",{"<< r2_es[3].range.first<<","<<r2_es[3].range.second<<"}}\tvalori teorici{"<< r2_teo[3].pid<<",{"<< r2_teo[3].range.first<<","<<r2_teo[3].range.second<<"}}\n";

  }else{
    cout<<"["<<__func__<<"] Test 2: numero di casi errato "<< r2_es.size()<<"\n";
  }

// ---
  vector<Custom_range> r3_es=crea_range2(SIZE,20,6);
  r3_teo.push_back({1,{0,19}});
  r3_teo.push_back({2,{20,39}});
  r3_teo.push_back({3,{40,59}});
  r3_teo.push_back({4,{60,79}});
  r3_teo.push_back({5,{80,99}});

  if(r3_es.size()==r3_teo.size()){
    if(cmp_custom_range(r3_es[0],r3_teo[0]))
      cout<<"["<<__func__<<"] Test 3: range 1 ok\n";
    else 
      cout<<"["<<__func__<<"] Test 3: range 1 errato , valori ritornati{"<<r3_es[0].pid<<",{"<< r3_es[0].range.first<<","<<r3_es[0].range.second<<"}}\tvalori teorici{"<< r3_teo[0].pid<<",{"<< r3_teo[0].range.first<<","<<r3_teo[0].range.second<<"}}\n";

    if(cmp_custom_range(r3_es[1],r3_teo[1]))
      cout<<"["<<__func__<<"] Test 3: range 2 ok\n";
    else 
      cout<<"["<<__func__<<"] Test 3: range 2 errato , valori ritornati{"<<r3_es[1].pid<<",{"<< r3_es[1].range.first<<","<<r3_es[1].range.second<<"}}\tvalori teorici{"<< r3_teo[1].pid<<",{"<< r3_teo[1].range.first<<","<<r3_teo[1].range.second<<"}}\n";

    if(cmp_custom_range(r3_es[2],r3_teo[2]))
      cout<<"["<<__func__<<"] Test 3: range 3 ok\n";
    else 
      cout<<"["<<__func__<<"] Test 3: range 3 errato , valori ritornati{"<<r3_es[2].pid<<",{"<< r3_es[2].range.first<<","<<r3_es[2].range.second<<"}}\tvalori teorici{"<< r3_teo[2].pid<<",{"<< r3_teo[2].range.first<<","<<r3_teo[2].range.second<<"}}\n";

    if(cmp_custom_range(r3_es[3],r3_teo[3]))
      cout<<"["<<__func__<<"] Test 3: range 4 ok\n";
    else 
      cout<<"["<<__func__<<"] Test 3: range 4 errato , valori ritornati{"<<r3_es[3].pid<<",{"<< r3_es[3].range.first<<","<<r3_es[3].range.second<<"}}\tvalori teorici{"<< r3_teo[3].pid<<",{"<< r3_teo[3].range.first<<","<<r3_teo[3].range.second<<"}}\n";

    if(cmp_custom_range(r3_es[4],r3_teo[4]))
      cout<<"["<<__func__<<"] Test 3: range 5 ok\n";
    else 
      cout<<"["<<__func__<<"] Test 3: range 5 errato , valori ritornati{"<<r3_es[4].pid<<",{"<< r3_es[4].range.first<<","<<r3_es[4].range.second<<"}}\tvalori teorici{"<< r3_teo[4].pid<<",{"<< r3_teo[4].range.first<<","<<r3_teo[4].range.second<<"}}\n";

  }else{
    cout<<"["<<__func__<<"] Test 3: numero di casi errato numero di nodi restituiti "<< r3_es.size()<<"\n";
    for(auto [pid,e]:r3_es){
      cout<<"["<<__func__<<"] pid="<<pid<<" ,f= "<<e.first<<" ,l= "<<e.second<<" \n";
    }

  }

}
int main(int argc,char *argv[]) {
//  uso_erase();

//	test_casi_banali();
//  test_2();
//  test_4();
  test_1();    
  return 0;
}

