OUT = out
SRC = src
INCLUDE = include
TEST= test
FLAGS=-Wall -g -I

compila_test_nodo:
	g++$(FLAGS) $(TEST)/test_nodo.cpp -o $(OUT)/test_nodo
compila:
	g++ $(FLAGS) ./$(INCLUDE) $(SRC)/main.cpp $(SRC)/simulation.cpp -o $(OUT)/main 
compila_ns:
	g++ $(FLAGS) ./$(INCLUDE) $(TEST)/ns.cpp  -o $(OUT)/ns 
ns:compila_ns
	./$(OUT)/ns 

esegui:compila
	./$(OUT)/main 

compila_test_lettura:
	g++ $(FLAGS) ./$(INCLUDE) $(TEST)/read_file.cpp -o $(TEST)/read

read:compila_test_lettura
	cd test; ./read 

clean:
	rm -v $(OUT)/test
