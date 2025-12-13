BIN = bin

all: $(BIN)/contar.out

$(BIN)/hiloprincipal.o: hiloprincipal.cc hilolector.h
	g++ -g -c hiloprincipal.cc -o $@

$(BIN)/hilolector.o: hilolector.cc hilolector.h hilocontador.h FileReader.h
	g++ -g -c hilolector.cc -o $@

$(BIN)/hilocontador.o: hilocontador.cc hilocontador.h
	g++ -g -c hilocontador.cc -o $@

$(BIN)/FileReader.o: FileReader.cc FileReader.h
	g++ -g -c FileReader.cc -o $@

$(BIN)/contar.out: $(BIN)/hiloprincipal.o $(BIN)/hilolector.o $(BIN)/hilocontador.o $(BIN)/FileReader.o
	g++ -g $^ -lpthread -o $@

clean:
	@rm -f *.o $(BIN)/*
