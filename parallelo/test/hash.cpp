
/*
 *
 * test invio hashmap in vari scenari
 * */

#include <mpi.h>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <vector>
using namespace std;

struct Pos{
  int x,y;
  bool operator<(const Pos& a) const {
    return ((this->x<a.x) || (this->y<a.y));
  }
  bool operator==(const Pos& a) const {
    return ((this->x==a.x) && (this->y==a.y));
  }

};
void base(){
 int my_rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size);   

    if (size < 2) {
        if (my_rank == 0) {
            std::cerr << "Il programma richiede almeno 2 processi.\n";
        }
        MPI_Finalize();
        return;
    }

    if (my_rank == 0) {
        // Processo Master
        map<Pos, int> map;
        map[{1, 0}] = 1; // Inserisce un elemento nella mappa

        // Serializzazione della mappa
        vector<int> buffer;
        for (const auto& e : map) {
            buffer.push_back(e.first.x);
            buffer.push_back(e.first.y);
            buffer.push_back(e.second);
        }

        // Invio del numero di elementi (dimensione del buffer)
        int buffer_size = buffer.size();
        MPI_Send(&buffer_size, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

        // Invio dei dati della mappa
        MPI_Send(buffer.data(), buffer_size, MPI_INT, 1, 1, MPI_COMM_WORLD);

        // Stampa dei dati inviati
        for (const auto& e : map) {
            std::cout << "[" << my_rank << "]: invio map[" << e.first.x << ", " << e.first.y << "] = " << e.second << "\n";
        }
        std::cout << "[" << my_rank << "]: dimensione map = " << map.size() << "\n";
    } else if (my_rank == 1) {
        // Processo Slave
        std::map<Pos, int> map;

        // Ricezione del numero di elementi
        int buffer_size;
        MPI_Recv(&buffer_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Ricezione dei dati della mappa
        std::vector<int> buffer(buffer_size);
        MPI_Recv(buffer.data(), buffer_size, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Deserializzazione della mappa
        for (size_t i = 0; i < buffer.size(); i += 3) {
            Pos pos = {buffer[i], buffer[i + 1]};
            int value = buffer[i + 2];
            map[pos] = value;
        }

        // Stampa dei dati ricevuti
        for (const auto& e : map) {
            std::cout << "[" << my_rank << "]: ricevo map[" << e.first.x << ", " << e.first.y << "] = " << e.second << "\n";
        }
    }

}

void vettore(){
 int my_rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // Ottiene il rank del processo
    MPI_Comm_size(MPI_COMM_WORLD, &size);   // Ottiene il numero totale di processi

    if (size < 2) {
        if (my_rank == 0) {
            std::cerr << "Il programma richiede almeno 2 processi.\n";
        }
        MPI_Finalize();
        return;
    }

    if (my_rank == 0) {
        // Processo Master
        std::map<Pos, int> map;
        map[{1, 0}] = 1; 
        map[{1, 2}] = 1; 

        // Serializzazione della mappa
        std::vector<int> buffer;
        for (const auto& e : map) {
            buffer.push_back(e.first.x);
            buffer.push_back(e.first.y);
            buffer.push_back(e.second);
        }

        // Invio del numero di elementi (dimensione del buffer)
        int buffer_size = buffer.size();
        MPI_Send(&buffer_size, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

        // Invio dei dati della mappa
        MPI_Send(buffer.data(), buffer_size, MPI_INT, 1, 1, MPI_COMM_WORLD);

        // Stampa dei dati inviati
        for (const auto& e : map) {
            std::cout << "[" << my_rank << "]: invio map[" << e.first.x << ", " << e.first.y << "] = " << e.second << "\n";
        }
        std::cout << "[" << my_rank << "]: dimensione map = " << map.size() << "\n";
    } else if (my_rank == 1) {
        // Processo Slave
        std::map<Pos, int> map;

        // Ricezione del numero di elementi
        int buffer_size;
        MPI_Recv(&buffer_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Ricezione dei dati della mappa
        std::vector<int> buffer(buffer_size);
        MPI_Recv(buffer.data(), buffer_size, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Deserializzazione della mappa
        for (size_t i = 0; i < buffer.size(); i += 3) {
            Pos pos = {buffer[i], buffer[i + 1]};
            int value = buffer[i + 2];
            map[pos] = value;
        }

        // Stampa dei dati ricevuti
        for (const auto& e : map) {
            std::cout << "[" << my_rank << "]: ricevo map[" << e.first.x << ", " << e.first.y << "] = " << e.second << "\n";
        }
    }

}

vector<Pos> get_Pos(){
  vector<Pos> ris;
  ris.push_back({0,0});
  ris.push_back({2,3});
  ris.push_back({3,4});
  ris.push_back({4,5});
  ris.push_back({5,6});
  ris.push_back({6,7});
  ris.push_back({7,8});
  ris.push_back({8,9});
  ris.push_back({9,10});
  ris.push_back({10,11});
  ris.push_back({0,0});
  ris.push_back({5,6});
  ris.push_back({2,3});
  return ris;
}
// simulo il comportamento di simula_turno

void test_intervalli(){
        vector<Pos> nodi_attivi = get_Pos();
        pair<int, int> r1(0, (nodi_attivi.size() / 2) - 1);
        pair<int, int> r2((nodi_attivi.size() / 2), nodi_attivi.size()-1 );
        cout<<"Verifica correttezza intervalli\nPrima meta dei nodi:\n"<<endl;
        for(int i=r1.first;i<=r1.second;i++)
          cout<<"("<<nodi_attivi[i].x<<","<<nodi_attivi[i].y<<")\n";
        std::cout << "Seconda meta del vettore:\n";
        for(int i=r2.first;i<=r2.second;i++)
          cout<<"("<<nodi_attivi[i].x<<","<<nodi_attivi[i].y<<")\n";
}
void comportamento_2(){

  int my_rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
  MPI_Comm_size(MPI_COMM_WORLD, &size);   

  if (size != 3) {
    if (my_rank == 0) {
      std::cerr << "Il programma richiede almeno 2 processi.\n";
    }
    MPI_Finalize();
    return;
  }

    vector<Pos> nodi_attivi = get_Pos();
  if (my_rank == 0) {
    int total_elements = nodi_attivi.size();
    pair<int, int> r1(0, (total_elements / 2) - 1);
    pair<int, int> r2((total_elements / 2), total_elements - 1);
    MPI_Send(&r1, 2, MPI_INT, 1, 0, MPI_COMM_WORLD);
    MPI_Send(&r2, 2, MPI_INT, 2, 0, MPI_COMM_WORLD);
    cout<<"Master : invio intervalli\n";
    int ris[2];
    

    MPI_Recv(&ris[0], 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&ris[1], 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    cout<<"Master: ricevuti messaggi slave\n";
    if(ris[1]==1 && ris[0]==1){
      for(int i=0;i<total_elements;i++)
        cout<<"Master:("<<nodi_attivi[i].x<<","<<nodi_attivi[i].y<<")\n";
    }

  }
  else {
    // Processi Slave
    std::pair<int, int> r;
    MPI_Recv(&r, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    cout << "["<<my_rank<<"]:Intervalli ricevuti\n";

    // Modifica la porzione di dati assegnata
    for (int i = r.first; i <= r.second; ++i) {
      nodi_attivi[i].x*=1;
      nodi_attivi[i].y*=1;
    }
    cout << "["<<my_rank<<"]: Lavoro terminato invio segnale terminazione\n";
    int tmp=1;
    MPI_Send(&tmp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

  }
    MPI_Barrier(MPI_COMM_WORLD);
    if(my_rank==0){
      map<Pos,int> tot;
      int total_elements = nodi_attivi.size();
      pair<int, int> r1(0, (total_elements / 2) - 1);
      pair<int, int> r2((total_elements / 2), total_elements );
      MPI_Send(&r1, 2, MPI_INT, 1, 0, MPI_COMM_WORLD);
      MPI_Send(&r2, 2, MPI_INT, 2, 0, MPI_COMM_WORLD);
      cout<<"Master : invio intervalli\n";
      for(int i=1;i<size;i++){
        int buf_size;
        MPI_Recv(&buf_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        vector<int> buffer(buf_size);
        MPI_Recv(buffer.data(), buf_size, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (size_t i = 0; i < buffer.size(); i += 3) {
            Pos pos = {buffer[i], buffer[i + 1]};
            int value = buffer[i + 2];
            if(tot.count(pos)==1)
              tot[pos]+=value;
            else 
              tot[pos]=value;

            cout<<"Master :Pos("<<pos.x<<","<<pos.y<<")="<<value<<"\tmap="<<tot.at(pos)<<"\n";
        }
      }
      /*
      map<Pos,int> map2,map3;
      int buf_size2,buf_size3;

      MPI_Recv(&buf_size2, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      vector<int> buffer2(buf_size2);
      MPI_Recv(buffer2.data(), buf_size2, MPI_INT, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for (size_t i = 0; i < buffer2.size(); i += 3) {
          Pos pos = {buffer2[i], buffer2[i + 1]};
          int value = buffer2[i + 2];
          map2[pos] = value;
      }

      MPI_Recv(&buf_size3, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      vector<int> buffer3(buf_size3);
      MPI_Recv(buffer3.data(), buf_size3, MPI_INT, 2, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for (size_t i = 0; i < buffer3.size(); i += 3) {
          Pos pos = {buffer3[i], buffer3[i + 1]};
          int value = buffer3[i + 2];
          map3[pos] = value;
      }
      cout<<"Dati inviati da slave1\n";

        for (const auto& e : map2) {
            std::cout << "[" << my_rank << "]: ricevo map[" << e.first.x << ", " << e.first.y << "] = " << e.second << "\n";
        }

      
      cout<<"Dati inviati da slave2\n";
        for (const auto& e : map3) {
            std::cout << "[" << my_rank << "]: ricevo map[" << e.first.x << ", " << e.first.y << "] = " << e.second << "\n";
        }
        */
      cout<<"Master dati totali trovati:\n";
        for (const auto& e : tot) {
            std::cout << "[" << my_rank << "]: ricevo map[" << e.first.x << ", " << e.first.y << "] = " << e.second << "\n";
        }
        
      //fusione mappa
    }
    else{
      std::pair<int, int> r;
      MPI_Recv(&r, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      cout << "["<<my_rank<<"]:Intervalli ricevuti\n";
      map<Pos,int> map;
      for(int i=r.first;i<r.second;i++){
        Pos tmp ={nodi_attivi[i].x,nodi_attivi[i].y};
        map[tmp] = 1; 
      }

       // if (my_rank==1){
          vector<int> buffer;
          for (const auto& e : map) {
            buffer.push_back(e.first.x);
            buffer.push_back(e.first.y);
            buffer.push_back(e.second);
          }
          // Invio del numero di elementi (dimensione del buffer)
          int buffer_size = buffer.size();

          MPI_Send(&buffer_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
          MPI_Send(buffer.data(), buffer_size, MPI_INT, 0, 1, MPI_COMM_WORLD);


          cout << "["<<my_rank<<"]:invio hashmap a master\n";
        //}


    }



}
int main(int argc,char** argv){
	MPI_Init(&argc, &argv);
//	base();
//	vettore();
// test_intervalli();
	comportamento_2();

	MPI_Finalize();
}



