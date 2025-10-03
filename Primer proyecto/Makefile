BIN = bin

all: $(BIN)/Monkeys.out

$(BIN)/Lock.o: Lock.h Lock.cc
	g++ -g -c Lock.cc -o $@

$(BIN)/Semaforo.o: Semaforo.h Semaforo.cc
	g++ -g -c Semaforo.cc -o $@

$(BIN)/Condition.o: Condition.h Condition.cc
	g++ -g -c Condition.cc -o $@

$(BIN)/Canopy.o: Canopy.h Canopy.cc
	g++ -g -c Canopy.cc -o $@

$(BIN)/Monkeys.o: Monkeys.cc Canopy.h
	g++ -g -c Monkeys.cc -o $@

$(BIN)/Monkeys.out: $(BIN)/Monkeys.o $(BIN)/Canopy.o $(BIN)/Lock.o $(BIN)/Condition.o $(BIN)/Semaforo.o
	g++ -g $^ -o $@

$(BIN)/Mutex.o: Mutex.h Mutex.cc
	g++ -g -c Mutex.cc -o $@

$(BIN)/Buzon.o: Buzon.h Buzon.cc Potato.h
	g++ -g -c Buzon.cc -o $@

$(BIN)/Pringles.o: Pringles.cc Potato.h
	g++ -g -c Pringles.cc -o $@

$(BIN)/Pringles.out: $(BIN)/Pringles.o $(BIN)/Buzon.o $(BIN)/Semaforo.o $(BIN)/Mutex.o $(BIN)/Lock.o
	g++ -g $^ -o $@

clean:
	@rm -f *.o $(BIN)/*
