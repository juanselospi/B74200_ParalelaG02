#
# Semana 05
#
# Dining philosophers monitor solution
#
# CI0117 Programación paralela y concurrente
#

all: filosofos.out

Lock.o: Lock.h Lock.cc
	g++ -g -c Lock.cc

Semaforo.o: Semaforo.h Semaforo.cc
	g++ -g -c Semaforo.cc

Condition.o: Condition.h Condition.cc
	g++ -g -c Condition.cc

Mesa.o: Mesa.h Mesa.cc
	g++ -g -c Mesa.cc

filosofos.o: filosofos.cc Mesa.h
	g++ -g -c filosofos.cc

filosofos.out: filosofos.o Mesa.o Lock.o Condition.o Semaforo.o
	g++ -g filosofos.o Mesa.o Lock.o Condition.o Semaforo.o -o filosofos.out

clean:
	@rm -f *.o *.out


