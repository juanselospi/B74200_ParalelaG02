BIN = bin
MEMO = Memoria
BUZ = Buzones

# Compila la version sin clases de enviar
$(BIN)/enviarSinClases: $(BUZ)/enviarSinClases.c
	gcc $(BUZ)/enviarSinClases.c -g -o $@

# Compila la version sin clases de recibir
$(BIN)/recibirSinClases: $(BUZ)/recibirSinClases.c
	gcc $(BUZ)/recibirSinClases.c -g -o $@

# Compila la version con clases de enviar
enviarConClases.o: $(BUZ)/enviarConClases.cc $(BUZ)/Buzon.h
	g++ -g -c $(BUZ)/enviarConClases.cc

recibirConClases.o: $(BUZ)/recibirConClases.cc $(BUZ)/Buzon.h
	g++ -g -c $(BUZ)/recibirConClases.cc

# Compila la clase Buzon
Buzon.o: $(BUZ)/Buzon.h $(BUZ)/Buzon.cc
	g++ -g -c $(BUZ)/Buzon.cc

# Enlaza los archivos t0-envia y Buzon para crear el ejecutable
$(BIN)/enviarConClases: enviarConClases.o Buzon.o
	g++ -g enviarConClases.o Buzon.o -o $@

# Enlaza los archivos t0-recibe y Buzon para crear el ejecutable
$(BIN)/recibirConClases: recibirConClases.o Buzon.o
	g++ -g recibirConClases.o Buzon.o -o $@

$(BIN)/pruebaSem: pruebaSem.o Semaforo.o
	g++ -g pruebaSem.o Semaforo.o -o $@

pruebaSem.o: $(MEMO)/pruebaSem.cc $(MEMO)/Semaforo.h
	g++ -c -g $(MEMO)/pruebaSem.cc

Semaforo.o: $(MEMO)/Semaforo.cc $(MEMO)/Semaforo.h
	g++ -c -g $(MEMO)/Semaforo.cc

sumaUno.o: $(MEMO)/sumaUno.cc $(MEMO)/Semaforo.h
	g++ -c -g $(MEMO)/sumaUno.cc

$(BIN)/sumaUno: sumaUno.o Semaforo.o
	g++ -g sumaUno.o Semaforo.o -o $@

PiPorSeries.o: $(MEMO)/PiPorSeries.cc $(MEMO)/Semaforo.h
	g++ -c -g $(MEMO)/PiPorSeries.cc

$(BIN)/PiPorSeries: PiPorSeries.o Semaforo.o
	g++ -g PiPorSeries.o Semaforo.o -o $@

clean:
	@rm -f *.o* $(BIN)/*

