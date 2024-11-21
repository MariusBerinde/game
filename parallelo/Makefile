OUT = out
SRC = src
INCLUDE = include
TEST= test
FLAGS=-Wall -g -fopenmp -I  
compila_test_nodo:
	g++$(FLAGS) $(TEST)/test_nodo.cpp -o $(OUT)/test_nodo
compila:
	g++ $(FLAGS) ./$(INCLUDE) $(SRC)/main.cpp $(SRC)/simulation.cpp -o $(OUT)/main 
compila_matrice:
	mpic++ $(TEST)/matrix.cpp   -o  $(OUT)/matrix

compila_send:
	mpic++ $(FLAGS) ./$(INCLUDE) $(TEST)/send.cpp $(SRC)/simulation.cpp -o $(OUT)/send
#	g++ $(FLAGS) ./$(INCLUDE) $(TEST)/send.cpp $(SRC)/simulation.cpp -o $(OUT)/send



esegui:compila
	./$(OUT)/main 

compila_test_lettura:
	g++ $(FLAGS) ./$(INCLUDE) $(TEST)/read_file.cpp -o $(TEST)/read

read:compila_test_lettura
	cd test; ./read 

matrice:compila_matrice 
	mpirun -np 4 $(OUT)/matrix	


send:compila_send 
	mpirun -np 3 $(OUT)/send	
#	./$(OUT)/main 

clean:
	rm -v $(OUT)/test
