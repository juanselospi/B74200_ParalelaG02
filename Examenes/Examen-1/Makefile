BIN = bin

$()/preguntaA: preguntaA.cc
	g++ -g preguntaA.cc -pthread -o $@

$(BIN)/Lock.o: Lock.h Lock.cc
	g++ -g -c Lock.cc -o $@

$(BIN)/Semaforo.o: Semaforo.h Semaforo.cc
	g++ -g -c Semaforo.cc -o $@

$(BIN)/Condition.o: Condition.h Condition.cc
	g++ -g -c Condition.cc -o $@

$(BIN)/monitorB.o: monitorB.h monitorB.cc
	g++ -g -c monitorB.cc -o $@

$(BIN)/preguntaB.o: preguntaB.cc monitorB.h
	g++ -g -c preguntaB.cc -o $@

$(BIN)/preguntaB.out: $(BIN)/preguntaB.o $(BIN)/monitorB.o $(BIN)/Lock.o $(BIN)/Condition.o $(BIN)/Semaforo.o
	g++ -g $^ -o $@

$(BIN)/Mutex.o: Mutex.h Mutex.cc
	g++ -g -c Mutex.cc -o $@

$(BIN)/Buzon.o: Buzon.h Buzon.cc
	g++ -g -c Buzon.cc -o $@

$(BIN)/H2OShM.o: H2OShM.cc Semaforo.h Mutex.h Lock.h
	g++ -g -c H2OShM.cc -o $@

$(BIN)/H2OShM.out: $(BIN)/H2OShM.o $(BIN)/Semaforo.o $(BIN)/Mutex.o $(BIN)/Lock.o
	g++ -g -o bin/H2OShM.out H2OShM.o Semaforo.o Lock.o Mutex.o -lcurses


$(BIN)/H2OBuz.o: H2OBuz.cc Buzon.h Mutex.h Lock.h
	g++ -g -c H2OBuz.cc -o $@

$(BIN)/H2OBuz.out: $(BIN)/H2OBuz.o $(BIN)/Buzon.o $(BIN)/Semaforo.o $(BIN)/Mutex.o $(BIN)/Lock.o
	g++ -g -o bin/H2OBuz.out H2OBuz.o Buzon.o Semaforo.o Mutex.o Lock.o -lcurses

clean:
	@rm -f *.o* $(BIN)/*
