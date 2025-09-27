all: pruebaSem.out

pruebaSem.out: pruebaSem.o Semaforo.o
	g++ -g pruebaSem.o Semaforo.o -o pruebaSem.out

pruebaSem.o: pruebaSem.cc Semaforo.h
	g++ -c -g pruebaSem.cc

Semaforo.o: Semaforo.cc Semaforo.h
	g++ -c -g Semaforo.cc

sumaUno.out: sumaUno.c
	gcc -g sumaUno.c -o sumaUno.out

PiPorSeries.out: PiPorSeries.c
	gcc -g PiPorSeries.c -o PiPorSeries.out

clean:
	@rm -f *.o*
