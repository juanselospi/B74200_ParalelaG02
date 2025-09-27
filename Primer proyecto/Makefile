all: Monkeys.out

Lock.o: Lock.h Lock.cc
	g++ -g -c Lock.cc

Semaforo.o: Semaforo.h Semaforo.cc
	g++ -g -c Semaforo.cc

Condition.o: Condition.h Condition.cc
	g++ -g -c Condition.cc

Canopy.o: Canopy.h Canopy.cc
	g++ -g -c Canopy.cc

Monkeys.o: Monkeys.cc Canopy.h
	g++ -g -c Monkeys.cc

Monkeys.out: Monkeys.o Canopy.o Lock.o Condition.o Semaforo.o
	g++ -g Monkeys.o Canopy.o Lock.o Condition.o Semaforo.o -o Monkeys.out

clean:
	@rm -f *.o *.out
