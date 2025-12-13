all: palindromo.out anilloFibonacci.out

palindromo.out: palindromo.o adn.o lcs.o
	g++ -g -fopenmp palindromo.o adn.o lcs.o -o palindromo.out

# PRIMERO dar permisos de ejecución con: chmod +x anilloFibonacci.out
# Para MPI ejecutar con: mpirun -np 6 ./anilloFibonacci.out
anilloFibonacci.out: anilloFibonacci.cc
	mpic++ -g anilloFibonacci.cc -o anilloFibonacci.out

clean:
	@rm -f *.o *.out