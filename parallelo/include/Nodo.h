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

};

