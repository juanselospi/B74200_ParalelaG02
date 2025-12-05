# Makefile para correr ejemplos personales

all: monosCanopyOMP.out

filosOMP.out: filosOMP.cc
	g++ -g -fopenmp filosOMP.cc -o filosOMP.out

# PRIMERO dar permisos de ejecución con: chmod +x filosMPI.out
# Para MPI ejecutar con: mpirun -np 6 ./filosMPI.out
filosMPI.out: filosMPI.cc
	mpic++ -g filosMPI.cc -o filosMPI.out

h2oOMP.out: h2oOMP.cc
	g++ -g -fopenmp h2oOMP.cc -o h2oOMP.out

# PRIMERO dar permisos de ejecución con: chmod +x h2oMPI.out
h2oMPI.out: h2oMPI.cc
	mpic++ -g h2oMPI.cc -o h2oMPI.out

productorConsumidorOMP.out: productorConsumidorOMP.cc
	g++ -g -fopenmp productorConsumidorOMP.cc -o productorConsumidorOMP.out

# PRIMERO dar permisos de ejecución con: chmod +x productorConsumidorMPI.out
# Para MPI ejecutar con: mpirun -np 7 ./productorConsumidorMPI.out
productorConsumidorMPI.out: productorConsumidorMPI.cc
	mpic++ -g productorConsumidorMPI.cc -o productorConsumidorMPI.out

lectoresEscritoresOMP.out: lectoresEscritoresOMP.cc
	g++ -g -fopenmp lectoresEscritoresOMP.cc -o lectoresEscritoresOMP.out

contarPrimosOMP.out: contarPrimosOMP.cc
	g++ -g -fopenmp contarPrimosOMP.cc -o contarPrimosOMP.out


sumaDeArregloOMP.out: sumaDeArregloOMP.cc
	g++ -g -fopenmp sumaDeArregloOMP.cc -o sumaDeArregloOMP.out

matrizXmatrizOMP.out: matrizXmatrizOMP.cc
	g++ -g -fopenmp matrizXmatrizOMP.cc -o matrizXmatrizOMP.out

matrizXvectorOMP.out: matrizXvectorOMP.cc
	g++ -g -fopenmp matrizXvectorOMP.cc -o matrizXvectorOMP.out

histogramaOMP.out: histogramaOMP.cc
	g++ -g -fopenmp histogramaOMP.cc -o histogramaOMP.out

sumaUnoOMP.out: sumaUnoOMP.cc
	g++ -g -fopenmp sumaUnoOMP.cc -o sumaUnoOMP.out

piPorSeriesOMP.out: piPorSeriesOMP.cc
	g++ -g -fopenmp piPorSeriesOMP.cc -o piPorSeriesOMP.out

productoPuntoOMP.out: productoPuntoOMP.cc
	g++ -g -fopenmp productoPuntoOMP.cc -o productoPuntoOMP.out

busquedaMaximoOMP.out: busquedaMaximoOMP.cc
	g++ -g -fopenmp busquedaMaximoOMP.cc -o busquedaMaximoOMP.out

# PRIMERO dar permisos de ejecución con: chmod +x scatterGatherMPI.out
# Para MPI ejecutar con: mpirun -np 5 ./scatterGatherMPI.out
scatterGatherMPI.out: scatterGatherMPI.cc
	mpic++ -g scatterGatherMPI.cc -o scatterGatherMPI.out

# PRIMERO dar permisos de ejecución con: chmod +x broadcastMatrizXvectorMPI.out
# Para MPI ejecutar con: mpirun -np 4 ./broadcastMatrizXvectorMPI.out
broadcastMatrizXvectorMPI.out: broadcastMatrizXvectorMPI.cc
	mpic++ -g broadcastMatrizXvectorMPI.cc -o broadcastMatrizXvectorMPI.out

# PRIMERO dar permisos de ejecución con: chmod +x reduceSumaArregloMPI.out
# Para MPI ejecutar con: mpirun -np 4 ./reduceSumaArregloMPI.out
reduceSumaArregloMPI.out: reduceSumaArregloMPI.cc
	mpic++ -g reduceSumaArregloMPI.cc -o reduceSumaArregloMPI.out

# PRIMERO dar permisos de ejecución con: chmod +x masterWorkersMatrizMPI.out
# Para MPI ejecutar con: mpirun -np 5 ./masterWorkersMatrizMPI.out
masterWorkersMatrizMPI.out: masterWorkersMatrizMPI.cc
	mpic++ -g masterWorkersMatrizMPI.cc -o masterWorkersMatrizMPI.out

# PRIMERO dar permisos de ejecución con: chmod +x papaBombaMPI.out
# Para MPI ejecutar con: mpirun -np 5 ./papaBombaMPI.out
papaBombaMPI.out: papaBombaMPI.cc
	mpic++ -g papaBombaMPI.cc -o papaBombaMPI.out

# PRIMERO dar permisos de ejecución con: chmod +x contarParesMPI.out
# Para MPI ejecutar con: mpirun -np 5 ./contarParesMPI.out
contarParesMPI.out: contarParesMPI.cc
	mpic++ -g contarParesMPI.cc -o contarParesMPI.out

# PRIMERO dar permisos de ejecución con: chmod +x sumaUnoMPI.out
# Para MPI ejecutar con: mpirun -np 6 ./sumaUnoMPI.out
sumaUnoMPI.out: sumaUnoMPI.cc
	mpic++ -g sumaUnoMPI.cc -o sumaUnoMPI.out

# PRIMERO dar permisos de ejecución con: chmod +x piPorSeriesMPI.out
# Para MPI ejecutar con: mpirun -np 6 ./piPorSeriesMPI.out
piPorSeriesMPI.out: piPorSeriesMPI.cc
	mpic++ -g piPorSeriesMPI.cc -o piPorSeriesMPI.out

sumaPrimosEx1OMP.out: sumaPrimosEx1OMP.cc
	g++ -g -fopenmp sumaPrimosEx1OMP.cc -o sumaPrimosEx1OMP.out

# PRIMERO dar permisos de ejecución con: chmod +x sumaPrimosEx1MPI.out
# Para MPI ejecutar con: mpirun -np 6 ./sumaPrimosEx1MPI.out
sumaPrimosEx1MPI.out: sumaPrimosEx1MPI.cc
	mpic++ -g sumaPrimosEx1MPI.cc -o sumaPrimosEx1MPI.out

binPackingOMP.out: binPackingOMP.cc
	g++ -g -fopenmp binPackingOMP.cc -o binPackingOMP.out

# PRIMERO dar permisos de ejecución con: chmod +x binPackingMPI.out
# Para MPI ejecutar con: mpirun -np 6 ./binPackingMPI.out
binPackingMPI.out: binPackingMPI.cc
	mpic++ -g binPackingMPI.cc -o binPackingMPI.out

monosCanopyOMP.out: monosCanopyOMP.cc
	g++ -g -fopenmp monosCanopyOMP.cc -o monosCanopyOMP.out

clean:
	@rm -f *.o *.out
