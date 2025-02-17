#include <cstddef>
#include <fstream>
#include <iostream>
#include <pstl/glue_algorithm_defs.h>
#include <string>
#include <vector>

using namespace std;
struct Pos {
  int x,y;
  string nome;
};

struct Config{
  int righe,colonne,tempo;
  vector<Pos> nodi;
};

string rmSpace(string val){
  int non_space_count = 0;
   // Traverse a val and if it is non space character then, place it at index non_space_count
    for (int i = 0; val[i] != '\0'; i++)
        if (val[i] != ' ')
        {
            val[non_space_count] = val[i];
            non_space_count++; // non_space_count incremented
        }    
    
    // Finally placing final character at the val end
    val[non_space_count] = '\0';
    return val;
}
Config read_file(){
  Config ris;
 // int righe=-1;
 // int col = -1;
 // int tempo = -1;
 // vector<Pos> nodi;
  ifstream file("./../src/config.txt");

  // String to store each line of the file.
  string line;

  if (file.is_open()) {
    // Read each line from the file and store it in the
    // 'line' variable.
    while (getline(file, line)) {
      if(!line.empty() && line[0] != '/'){

        bool  sup = line.find("righe") != string::npos;
        if(sup){

          string l = rmSpace(line);
          auto pos_eq=  l.find("=");
          auto pos_new_line=  l.find("\n");
          string nrs = l.substr(pos_eq+1,pos_new_line-pos_eq-1);
          printf("\tposizione uguale=%ld\tposzione line=%ld\n\n",pos_eq,pos_new_line);
          //cout <<"numero trovato ="<<nrs<<"\n";
          int nr = stoi(nrs);
         // righe = nr;
          ris.righe= nr;
        }
        sup=line.find("colonne") != string::npos;
        if(sup){
          string l = rmSpace(line);
          auto pos_eq=  l.find("=");
          auto pos_new_line=  l.find("\n");
          string nrs = l.substr(pos_eq+1,pos_new_line-pos_eq-1);
          //printf("\tposizione uguale=%ld\tposzione line=%ld\n\n",pos_eq,pos_new_line);
          //cout <<"numero trovato ="<<nrs<<"\n";
          int nr = stoi(nrs);
          //col= nr;
          ris.colonne=nr;

        }
        sup=line.find("turni") != string::npos;
        if(sup){
          string l = rmSpace(line);
          auto pos_eq=  l.find("=");
          auto pos_new_line=  l.find("\n");
          string nrs = l.substr(pos_eq+1,pos_new_line-pos_eq-1);
          //printf("\tposizione uguale=%ld\tposzione line=%ld\n\n",pos_eq,pos_new_line);
          //cout <<"numero trovato ="<<nrs<<"\n";
          int nr = stoi(nrs);
          //col= nr;
          ris.tempo=nr;

        }
        sup=line.find("Attivi") != string::npos;
        
        if(sup){
          
          while (getline(file, line)) {

            if(!line.empty() && line[0] != '/'){
              int tmp_x=-1,tmp_y=-1;
              string l = rmSpace(line);
              size_t pos_1 = l.find("(");
              size_t pos_2 = l.find(",");
              size_t pos_3 = l.find(")");
              string nome = l.substr(0,pos_1);
              string sup = l.substr(pos_1+1,pos_2-pos_1);
              tmp_x = stoi(sup);
              sup = l.substr(pos_2+1,pos_3-1-pos_2);
              tmp_y = stoi(sup);
              ris.nodi.push_back({tmp_x,tmp_y,nome});
            }
          }

        }
        
        

      } 
    }

    file.close();
/*
    ris.righe=righe; ris.colonne=col;
    ris.tempo=tempo;
    ris.nodi = nodi;*/
    return ris;
  }
  else {
    cerr << "Unable to open file!" << endl;
  }

  return ris;
}

void test_find(){
  string linea = "A ( 5 , 12 )";
  //bool  ris = linea.find("righe") != string::npos;
  int tmp_x,tmp_y=-1;
  string l = rmSpace(linea);
  size_t pos_1 = l.find("(");
  size_t pos_2 = l.find(",");
  size_t pos_3 = l.find(")");
  string nome = l.substr(0,pos_1);
  string sup = l.substr(pos_1+1,pos_2-pos_1);
  tmp_x = stoi(sup);
  sup = l.substr(pos_2+1,pos_3-1-pos_2);
    tmp_y = stoi(sup);
  
  cout<<"nome nodo:"<<nome<<"\t x:"<<tmp_x<<"\ty:"<<tmp_y<<"\n";
  /*
  cout << "risultato find con parola righe ="<<ris<<"\n"<< endl;
  ris = linea.find("Numero") != string::npos;
  cout << "risultato find con parola 8 ="<<ris<<"\n"<< endl;
  ris = linea.find("8") != string::npos;
  cout << "risultato find con parola 8 ="<<ris<<"\n"<< endl;
  cout <<"test eliminazione spazi  e ritrovamento posizione\n";
  string l = rmSpace(linea);
  cout <<" linea senza spazi ="<<l<<"\n";

  auto pos_eq=  l.find("=");
  auto pos_new_line=  l.find("\n");
  string nrs = l.substr(pos_eq+1,pos_new_line-pos_eq-1);
  //printf("\tposizione uguale=%ld\tposzione line=%ld\n\n",pos_eq,pos_new_line);
  cout <<"numero trovato ="<<nrs<<"\n" ;
  //nrs.erase(0, nrs.find_first_not_of(" ")); 
  //nrs.erase(nrs.find_last_not_of(" ") + 1);
  int nr = stoi(nrs);
  cout << "striga originale "<<linea<<"\n";
  printf("nr trovato con estremi=%d\n",nr);
*/
  

}

void test_read(){
  int righe_teo = 5;
  int col_teo = 5;
  int tempo_teo = 5;
  Pos A={0,0,"A"};
  Pos B={0,1,"B"};
  auto lettura = read_file();

  if (lettura.righe == righe_teo)
    cout<<"NUMERO RIGHE OK\n";
  else
    cout<<"NUMERO RIGHE ERROR\n";


  if (lettura.colonne == col_teo)
    cout<<"NUMERO COLONNE OK\n";
  else
    cout<<"NUMERO COLONNE ERROR\n";


  if (lettura.tempo == tempo_teo)
    cout<<"NUMERO TEMPO OK\n";
  else
    cout<<"NUMERO TEMPO ERROR\n";


  if(lettura.nodi.size() != 2){
    cout<<"NUMERO NODI ERROR\n";
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
      cout<<"Punto A trovato\n";
    }
    else 
      cout<<"Punto A non trovato\n";


    if(esiti[0]){
      cout<<"Punto B trovato\n";
    }
    else 
      cout<<"Punto B non trovato\n";
  }

}

int main(){
  //read_file();
  //test_find();
  test_read();
  

  return 0;
}
