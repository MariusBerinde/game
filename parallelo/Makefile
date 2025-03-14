OUT = out
SRC = src
INCLUDE = include
TEST= test
FLAGS= -Wall -O3  -g  -I  #-O2 -fopenmp

compila_test_nodo:
	g++$(FLAGS) $(TEST)/test_nodo.cpp -o $(OUT)/test_nodo
compila:
	g++ $(FLAGS) ./$(INCLUDE) $(SRC)/main.cpp $(SRC)/simulation.cpp -o $(OUT)/main 

compila_p:
	mpic++ $(FLAGS) ./$(INCLUDE) $(SRC)/main.cpp $(SRC)/simulation.cpp -o $(OUT)/main 
compila_matrice:
	mpic++ $(TEST)/matrix.cpp   -o  $(OUT)/matrix

compila_send:
	mpic++ $(FLAGS) ./$(INCLUDE) $(TEST)/send.cpp $(SRC)/simulation.cpp -o $(OUT)/send
#	g++ $(FLAGS) ./$(INCLUDE) $(TEST)/send.cpp $(SRC)/simulation.cpp -o $(OUT)/send

compila_sched:
	mpic++ $(FLAGS) ./$(INCLUDE) $(TEST)/sched.cpp $(SRC)/simulation.cpp -o $(OUT)/sched	
	

sched:compila_sched
	mpirun -np 3 $(OUT)/sched	
compila_hash:
	mpic++ -Wall  ./$(TEST)/hash.cpp   -o  $(OUT)/hash

compila_test:
	g++ $(FLAGS) ./$(INCLUDE) $(SRC)/test_config.cpp $(SRC)/simulation.cpp -o $(OUT)/test_config 

complila_distr:$(TEST)/distr.cpp
	mpic++ $(FLAGS) ./$(INCLUDE) $(TEST)/distr.cpp $(SRC)/simulation.cpp -o $(OUT)/distr
U ?= 0
test:compila_test 
	./$(OUT)/test_config $(U)

esegui:compila
	./$(OUT)/main 

distr:complila_distr
	mpirun -np 1 ./$(OUT)/distr 

esegui_p:compila_p
	mpirun -np 3 ./$(OUT)/main 

compila_test_lettura:
	g++ $(FLAGS) ./$(INCLUDE) $(TEST)/read_file.cpp -o $(TEST)/read

read:compila_test_lettura
	cd test; ./read 

matrice:compila_matrice 
	mpirun -np 4 $(OUT)/matrix	


hash:compila_hash
	mpirun -np 3 $(OUT)/hash	

send:compila_send 
	mpirun -np 4 $(OUT)/send	
#	./$(OUT)/main 

clean:
	rm -v $(OUT)/*
