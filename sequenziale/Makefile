OUT = out
SRC = src
INCLUDE = include
TEST= test
FLAGS=-Wall -g

compila_test_nodo:
	g++$(FLAGS) $(TEST)/test_nodo.cpp -o $(OUT)/test_nodo


disegna_totale:compila_disegno_totale
	./$(OUT)/disegno_totale
clean:
	rm -v $(OUT)/test
