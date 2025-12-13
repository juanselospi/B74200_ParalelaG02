# Makefile para Semana-08

AtomicInt.out: AtomicInt.cc
	g++ -g AtomicInt.cc -lpthread -o AtomicInt.out

call_once.out: call_once.cc
	g++ -g --std=c++11 call_once.cc -lpthread -o call_once.out

latch.out: latch.cc
	g++ -g --std=c++20 latch.cc -lpthread -o latch.out

filosofosPT.out: filosofosPT.cc
	g++ -g -std=c++17 filosofosPT.cc -lpthread -o filosofosPT.out

sumaUnoPT.out: sumaUnoPT.cc
	g++ -g sumaUnoPT.cc -lpthread -o sumaUnoPT.out

omp_sin_sum.out: omp_sin_sum.c
	gcc -g -Wall -fopenmp -I. -o omp_sin_sum omp_sin_sum.c

sumaUnoOMP.out: sumaUnoOMP.cc
	g++ -g -fopenmp sumaUnoOMP.cc -o sumaUnoOMP.out

PiPorSeriesOMP.out: PiPorSeriesOMP.cc
	g++ -g -fopenmp PiPorSeriesOMP.cc -o PiPorSeriesOMP.out

omp_mat_vec.out: omp_mat_vec.cc
	g++ -g -fopenmp omp_mat_vec.cc -o omp_mat_vec.out

clean:
	rm -f *.out *.o
