#include "../include/simulation.h"

void test_creation(){

    // Creazione di un'istanza di Simulation con 5 righe, 5 colonne e 10 unità di tempo
    Simulation sim(5, 5, 10);

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
int main() {
//  test_creation();
  test_get_vicini();
    return 0;
}

