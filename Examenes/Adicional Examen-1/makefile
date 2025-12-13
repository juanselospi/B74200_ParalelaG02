BIN = bin

all: $(BIN)/carrito.out

$(BIN)/tienda.o: tienda.h tienda.cc
	g++ -g -c tienda.cc -o $@

$(BIN)/carrito.o: carrito.cc tienda.h
	g++ -g -c carrito.cc -o $@

$(BIN)/carrito.out: $(BIN)/carrito.o $(BIN)/tienda.o
	g++ -g $^ -o $@

clean:
	@rm -f *.o $(BIN)/*
