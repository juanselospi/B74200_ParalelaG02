all: mediosCUDA.out

%.o: %.cc
	g++ -g -c $< -fopenmp

%.o: %.c
	gcc -g -c $<

%.o: %.cu
	nvcc -g -c $<

pruebaPuntos.out: Punto.o VectorPuntos.o pruebaPuntos.o
	g++ -g -lm pruebaPuntos.o VectorPuntos.o Punto.o -o pruebaPuntos.out

mediosCUDA.out: Punto.o VectorPuntos.o mediosCUDA.o
	nvcc -g mediosCUDA.o VectorPuntos.o Punto.o -o mediosCUDA.out -Xcompiler -fopenmp

clean:
	rm -f *.o *.out
