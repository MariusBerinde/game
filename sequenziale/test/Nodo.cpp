#include <iostream>
#include <vector>
#include <memory>
enum Stato {live,dead};
class Nodo{
	private:
		int x;
		int y;
		Stato stato;
		std::vector<std::shared_ptr<Nodo>> vicini;  // Vector di shared_ptr
	public:
		Nodo(int xP,int yP,Stato statoIniziale = Stato::dead):
			x(xP),y(yP),stato(statoIniziale){
				vicini.reserve(8);

			}
		int getX(){return x;}
		int getY(){return y;}
		void uccidiNodo(){
			stato = Stato::dead;
		}

		void svegliaNodo(){
			stato = Stato::live;
		}
		void addVicino(const std::shared_ptr<Nodo>& vicino){
			if (vicini.size() < 8) {
				vicini.push_back(vicino);
			} else {
				std::cerr << "Impossibile aggiungere più di 8 vicini!" << std::endl;
			}
		} 
		void stampaInfo() const {
			std::cout << "Nodo(" << x << ", " << y << ") - Stato: "
				<< (stato == Stato::live ? "live" : "dead") << std::endl;
			std::cout << "Numero di vicini: " << vicini.size() << std::endl;
		}
		// Metodo per stampare i vicini del nodo
		void stampaVicini() const {
			std::cout << "Vicini del nodo (" << x << ", " << y << "):" << std::endl;
			if (vicini.empty()) {
				std::cout << "Nessun vicino." << std::endl;
			} else {
				for (const auto& vicino : vicini) {
					if (vicino) {
						std::cout << "  Nodo(" << vicino->x << ", " << vicino->y << ") - Stato: "
							<< (vicino->stato == Stato::live ? "live" : "dead") << std::endl;
					}
				}
			}
		}

		std::vector<std::shared_ptr<Nodo>>& getVicini(){	return vicini;}


};


int main(){
	auto nodo1 = std::make_shared<Nodo>(0,0);
	auto nodo2 = std::make_shared<Nodo>(0,1);
	auto nodo3 = std::make_shared<Nodo>(0,2);

	nodo1->stampaInfo();
	nodo1->svegliaNodo();
	nodo1->stampaInfo();
	nodo1->addVicino(nodo2);
	nodo1->addVicino(nodo3);
	nodo1->stampaInfo();
	nodo1->stampaVicini();

	auto& vicini_n1 = nodo1->getVicini();
	if(!vicini_n1.empty()){
		for (const auto& v : vicini_n1) {
				std::cout<<"nodo="<<v->getX()<<","<<v->getY()<<"\n"<<std::endl;
				}
	}
	else
		std::cout<<"nessun vicino"<<std::endl;


}

