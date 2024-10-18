OUT = out
SRC = src
INCLUDE = include
TEST= test
FLAGS=-Wall -g -I

compila_test_nodo:
	g++$(FLAGS) $(TEST)/test_nodo.cpp -o $(OUT)/test_nodo
compila:
	g++ $(FLAGS) ./$(INCLUDE) $(SRC)/main.cpp $(SRC)/simulation.cpp -o $(OUT)/main 

esegui:compila
	./$(OUT)/main 

disegna_totale:compila_disegno_totale
	./$(OUT)/disegno_totale
clean:
	rm -v $(OUT)/test
