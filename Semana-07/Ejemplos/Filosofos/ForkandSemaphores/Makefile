# CI-0117 Programacion Concurrente
# Makefile Semana-06
#
# Dining philosophers problem with fork and NON global variables
#
# Date: 2020/May/16
#

all: philoFork

Lock.o: Lock.h Lock.cc
	g++ -g -c Lock.cc

Mutex.o: Mutex.h Mutex.cc
	g++ -g -c Mutex.cc

Semaphore.o: Semaphore.h Semaphore.cc
	g++ -g -c Semaphore.cc

philoFork-gv.o: philoFork-gv.cc Semaphore.h Mutex.h
	g++ -g -c philoFork-gv.cc

philoFork-ngv.out: philoFork-ngv.c
	gcc -g philoFork-ngv.c -o philoFork-ngv.out -lcurses

philoFork-gv.out: philoFork-gv.o Semaphore.o Mutex.o
	g++ -g -o philoFork-gv.out philoFork-gv.o Semaphore.o Lock.o Mutex.o -lcurses

