#include <iostream>
#include <vector>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

const int MAX_ROW=5;
const int MAX_COL=5;
const int MAX_TIME=5;
enum Stato {live,dead};
struct Nodo {
    int x, y;           // Coordinate (i, j)
    Stato* stato;       // Puntatore allo stato della cella

    // Funzione toString() per convertire i valori del Nodo in una stringa
    std::string toString() const {
        std::ostringstream oss;
        oss << "Nodo (" << x << ", " << y << ") stato: " << (*stato == live ? "live" : "dead");
        return oss.str();
    }
};

int main () {
 
 std::cout << "test creazione nuova struttura dati\n"; 
 Stato map[MAX_ROW][MAX_TIME][MAX_TIME];
 int actual_time = 0;
 for(int i=0;i<MAX_ROW;i++){
   for(int j=0;j<MAX_COL;j++){
    map[i][j][actual_time] = dead;
   }
 }

 std::cout << "Matrice a t = 0\n";

 for(int i=0;i<MAX_ROW;i++){
   for(int j=0;j<MAX_COL;j++){
     printf("m[%d][%d]=%d\t",i,j,map[i][j][actual_time]);
   }
   std::cout << "\n";
 }

 // provo a creare un vettore di vettori di stati
 std::vector<std::vector<Nodo>> activeNodes(MAX_TIME);
 //inserisco i nodi attivi 
 map[0][0][actual_time] = live;
 map[0][4][actual_time] = live;
 map[4][0][actual_time] = live;
 map[4][4][actual_time] = live;

 Nodo a={0,0,&map[0][0][actual_time]};
 Nodo b={0,4,&map[0][4][actual_time]};
 Nodo c={4,0,&map[4][0][actual_time]};
 Nodo d={4,4,&map[4][4][actual_time]};
 activeNodes[actual_time].push_back(a);
 activeNodes[actual_time].push_back(b);
 activeNodes[actual_time].push_back(c);
 activeNodes[actual_time].push_back(d);

std::cout << "matrice dopo init:\n";

 for(int i=0;i<MAX_ROW;i++){
   for(int j=0;j<MAX_COL;j++){
     printf("m[%d][%d]=%d\t",i,j,map[i][j][actual_time]);
   }
   std::cout << "\n";
 }

 std::cout << "Matrice nodi attivi:\n";
 std::vector<Nodo> activeNodesNow = activeNodes.at(actual_time);
 for(int i=0;i<activeNodes[actual_time].size();i++){
   std::cout<<activeNodesNow.at(i).toString()<<"\n";
 }
 
}
