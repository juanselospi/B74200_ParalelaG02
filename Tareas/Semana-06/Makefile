# PThreads makefile example
#
# Author: CI0117 Programacion Paralela y Concurrente
# Date: 2020/Set/21
#
#  Pueden llamarlo para crear los .o individualmente
#   make Condition.o
#   make Lock.o
#   etc.

all: filosofos.out
# all: AguaPT.out

%.o: %.cc
	g++ -g -c $<

%.o: %.c
	gcc -g -c $<

sumaUnoPT.out: sumaUnoPT.o
	g++ -g -lpthread sumaUnoPT.o -o sumaUnoPT.out

AguaPT.out: AguaPT.o Semaforo.o
	g++ -g -lpthread AguaPT.o Semaforo.o -o AguaPT.out

Mutex.o: Mutex.h Mutex.cc
	g++ -g -c Mutex.cc -pthread

Lock.o: Lock.h Lock.cc
	g++ -g -c Lock.cc -pthread

Semaforo.o: Semaforo.h Semaforo.cc
	g++ -g -c Semaforo.cc -pthread

Condition.o: Condition.h Condition.cc
	g++ -g -c Condition.cc -pthread

Barrier.o: Barrier.h Barrier.cc
	g++ -g -c Barrier.cc -pthread

Mesa.o: Mesa.h Mesa.cc
	g++ -g -c Mesa.cc -pthread

filosofos.o: filosofos.cc Mesa.h
	g++ -g -c filosofos.cc -pthread

filosofos.out: filosofos.o Mesa.o Lock.o Condition.o Barrier.o Semaforo.o Mutex.o
	g++ -g filosofos.o Mesa.o Lock.o Condition.o Barrier.o Semaforo.o Mutex.o -o filosofos.out -pthread

clean:
	@rm -f *.o *.out
