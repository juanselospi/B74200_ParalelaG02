# CI-0117 Programacion Concurrente

all: philoPT

diningPh.o: diningPh.h diningPh.cc
	g++ -g -c diningPh.cc

philoPT.o: philoPT.cc diningPh.h
	g++ -g -c philoPT.cc

philoPT.out: philoPT.o diningPh.o
	g++ -g diningPh.o philoPT.o -pthread -o philoPT.out

