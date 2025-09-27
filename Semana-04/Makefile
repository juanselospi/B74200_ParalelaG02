# CI-0117 Programacion Concurrente
# Makefile Semana-04
#
# Dining philosophers problem with fork and NON global variables
#
# Date: 2025/Ago/29
#

all: filoFork-ngv.out filoFork-gv.out filoFork-gvBuz.out H2OShM.out

Lock.o: Lock.h Lock.cc
	g++ -g -c Lock.cc

Mutex.o: Mutex.h Mutex.cc
	g++ -g -c Mutex.cc

Semaforo.o: Semaforo.h Semaforo.cc
	g++ -g -c Semaforo.cc

filoFork-gv.o: filoFork-gv.cc Semaforo.h Mutex.h Lock.h
	g++ -g -c filoFork-gv.cc

filoFork-ngv.out: filoFork-ngv.c
	gcc -g filoFork-ngv.c -o bin/filoFork-ngv.out -lcurses

filoFork-gv.out: filoFork-gv.o Semaforo.o Mutex.o Lock.o
	g++ -g -o bin/filoFork-gv.out filoFork-gv.o Semaforo.o Lock.o Mutex.o -lcurses

Buzon.o: Buzon.h Buzon.cc
	g++ -g -c Buzon.cc

filoFork-gvBuz.o: filoFork-gvBuz.cc Buzon.h Mutex.h Lock.h
	g++ -g -c filoFork-gvBuz.cc

filoFork-gvBuz.out: filoFork-gvBuz.o Buzon.o Semaforo.o Mutex.o Lock.o
	g++ -g -o bin/filoFork-gvBuz.out filoFork-gvBuz.o Buzon.o Semaforo.o Mutex.o Lock.o -lcurses

H2OShM.o: H2OShM.cc Semaforo.h Mutex.h Lock.h
	g++ -g -c H2OShM.cc

H2OShM.out: H2OShM.o Semaforo.o Mutex.o Lock.o
	g++ -g -o bin/H2OShM.out H2OShM.o Semaforo.o Lock.o Mutex.o -lcurses


H2OBuz.o: H2OBuz.cc Buzon.h Mutex.h Lock.h
	g++ -g -c H2OBuz.cc

H2OBuz.out: H2OBuz.o Buzon.o Semaforo.o Mutex.o Lock.o
	g++ -g -o bin/H2OBuz.out H2OBuz.o Buzon.o Semaforo.o Mutex.o Lock.o -lcurses

clean:
	@rm -f *.o bin/*
