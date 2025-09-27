# Compila la version sin clases de enviar
enviarSinClases: enviarSinClases.c
	gcc enviarSinClases.c -g -o bin/enviarSinClases

# Compila la version sin clases de recibir
recibirSinClases: recibirSinClases.c
	gcc recibirSinClases.c -g -o bin/recibirSinClases

# Compila la version con clases de enviar
enviarConClases.o: enviarConClases.cc Buzon.h
	g++ -g -c enviarConClases.cc

recibirConClases.o: recibirConClases.cc Buzon.h
	g++ -g -c recibirConClases.cc

# Compila la clase Buzon
Buzon.o: Buzon.h Buzon.cc
	g++ -g -c Buzon.cc

# Enlaza los archivos t0-envia y Buzon para crear el ejecutable
t0-envia: t0-envia.o Buzon.o
	g++ t0-envia Buzon.o -o bin/t0-envia

# Enlaza los archivos t0-recibe y Buzon para crear el ejecutable
recibirConClases: recibirConClases.o Buzon.o
	g++ -g recibirConClases.o Buzon.o -o bin/recibirConClases

# Enlaza los archivos enviarConClases y Buzon para crear el ejecutable
enviarConClases: enviarConClases.o Buzon.o
	g++ -g enviarConClases.o Buzon.o -o bin/enviarConClases

# Esto compila pruebaShM
pruebaShM: pruebaShM.o Semaforo.o
	g++ -g pruebaShM.o Semaforo.o -o bin/pruebaShM

pruebaShM.o: pruebaShM.cc Semaforo.h
	g++ -c -g pruebaShM.cc

Semaforo.o: Semaforo.cc Semaforo.h
	g++ -c -g Semaforo.cc

# Esto compila PiPorSeries
PiPorSeries.o: PiPorSeries.cc Semaforo.h
	g++ -c -g PiPorSeries.cc

PiPorSeries: PiPorSeries.o Semaforo.o
	g++ -g PiPorSeries.o Semaforo.o -o bin/PiPorSeries

# Esto compila sumaUno
sumaUno.o: sumaUno.cc Semaforo.h
	g++ -c -g sumaUno.cc

sumaUno: sumaUno.o Semaforo.o
	g++ -g sumaUno.o Semaforo.o -o bin/sumaUno

# Compila la version sumaUnoBuzon
sumaUnoBuzon.o: sumaUnoBuzon.cc Buzon.h
	g++ -g -c sumaUnoBuzon.cc

# Enlaza sumaUnoBuzon.o y Buzon.o
sumaUnoBuzon: sumaUnoBuzon.o Buzon.o
	g++ -g sumaUnoBuzon.o Buzon.o -o bin/sumaUnoBuzon

# Compila la version PiPorSeriesBuzon
PiPorSeriesBuzon.o: PiPorSeriesBuzon.cc Buzon.h
	g++ -g -c PiPorSeriesBuzon.cc

# Enlaza PiPorSeriesBuzon.o y Buzon.o
PiPorSeriesBuzon: PiPorSeriesBuzon.o Buzon.o
	g++ -g PiPorSeriesBuzon.o Buzon.o -o bin/PiPorSeriesBuzon

clean:
	@rm -f *.o bin/*
