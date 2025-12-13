/**
 *  Calcula el numero PI utilizando una serie "infinita" (version OpenMP)
 *  Debe recibir la cantidad de iteraciones como parametro
 * 
 *  pi = (-1)^i x 4 / (2xi + 1)
 * 
 *  Adaptación de PiPorSeriesPT.cc a OpenMP con DEBUG
 *
 *  Compilar con:
 *      make PiPorSeriesOMP.out
 *  Ejecutar con:
 *      ./PiPorSeriesOMP.out [num_terminos] [num_hilos]
 *  [DEBUG]: Compilar con: 
 *      g++ -fopenmp -DDEBUG PiPorSeriesOMP.cc -o PiPorSeriesOMP.out
 *  [DEBUG]: Ejecutar con:
 *      ./PiPorSeriesOMP.out [num_terminos] [num_hilos]
 */

#include <iostream>
#include <omp.h>
#include <cmath>

using namespace std;

double pi = 0;

#ifdef DEBUG
int* iterations;
#endif

/**
  *   Serial calculation
 **/
int serialTest( long terminos ) {
   long termino;
   double casiPi = 0;
   double alterna = 4;
   long divisor = 0;

//printf( "Inicia en 0,  termina en %ld \n", terminos - 1 );
   for ( termino = 0; termino < terminos; termino++ ) {
      divisor = termino + termino + 1;		// 2 x termino + 1
      casiPi += alterna/divisor;		// 4 / (2xi + 1)
//printf( "CasiPi = %.20g\n", casiPi );
      alterna *= -1;				// Pasa de 4 a -4 y viceversa, para realizar la aproximacion de los terminos
   }

   printf( "Serial version PI:  \033[91m %.20g \033[0m ", casiPi );

   return 0;

}

/**
  *   test paralelo sin condicion de carrera usando seccion critica
 **/
double parallelPiCritical( long terminos, int hilos ) {
    pi = 0.0;

#ifdef DEBUG
    iterations = new int[ terminos ];
#endif

    int base = terminos / hilos;
    int resto = terminos % hilos;

    #pragma omp parallel num_threads( hilos )
    {
        int tid = omp_get_thread_num();
        long inicio, fin;

        long base = terminos / hilos;
        long resto = terminos % hilos;

        if( tid < resto ) {

            inicio = tid * ( base + 1 );
            fin = inicio + base;
        } else {

            inicio = tid * base + resto;
            fin = inicio + base - 1;
        }

        double sumaParcial = 0.0;

        for( long i = inicio; i <= fin; i++ ) {

            double termino;
            if( i % 2 == 0 ) {

                termino = 4.0 / ( 2*i + 1 );
            } else {

                termino = -4.0 / ( 2*i + 1 );
            }

            sumaParcial += termino;

#ifdef DEBUG
            iterations[ i ] = tid;
#endif

        }
        
        #pragma omp critical
        {
            pi += sumaParcial;
        }

    }

#ifdef DEBUG
    printf( "\n[DEBUG] Asignacion de iteraciones (CRITICAL):\n" );
    printf( "Thread\t\tIteraciones\n" );
    printf( "------\t\t-----------\n" );

    int hilo_actual = iterations[ 0 ];
    int inicioIter = 0;
    int finIter = 0;

    for( long i = 0; i < terminos; i++ ) {

        if( iterations[ i ] == hilo_actual ) {

            finIter = i;
        } else {

            printf( "%3d\t%4d a %4d\n", hilo_actual, inicioIter, finIter );
            hilo_actual = iterations[i];
            inicioIter = finIter = i;
        }
    }

    printf( "%3d\t%4d a %4d\n", hilo_actual, inicioIter, finIter );

    delete[] iterations;
#endif

    return pi;
}

/*
 *  version alternativa sin condiciones de carrera usando reduccion
 */
double parallelPiReduction( long terminos, int hilos ) {
    double resultado = 0.0;

#ifdef DEBUG
    iterations = new int[ terminos ];
#endif

    int base = terminos / hilos;
    int resto = terminos % hilos;

    #pragma omp parallel for num_threads( hilos ) reduction( +:resultado )
    for( long i = 0; i < terminos; i++ ) {
        double termino;

        if( i % 2 == 0 ) {

            termino = 4.0 / ( 2*i + 1 );
        } else {

            termino = -4.0 / ( 2*i + 1 );
        }

        resultado += termino;

#ifdef DEBUG
        iterations[ i ] = omp_get_thread_num();
#endif

    }

#ifdef DEBUG
    printf( "\n[DEBUG] Asignacion de iteraciones (REDUCTION):\n" );
    printf( "Thread\t\tIteraciones\n" );
    printf( "------\t\t-----------\n" );

    int hilo_actual = iterations[ 0 ];
    int inicioIter = 0;
    int finIter = 0;

    for( long i = 0; i < terminos; i++ ) {

        if( iterations[ i ] == hilo_actual ) {

            finIter = i;
        } else {

            printf( "%3d\t%4d a %4d\n", hilo_actual, inicioIter, finIter );
            hilo_actual = iterations[ i ];
            inicioIter = finIter = i;
        }
    }

    printf( "%3d\t%4d a %4d\n", hilo_actual, inicioIter, finIter );

    delete[] iterations;
#endif

    return resultado;
}

int main( int argc, char **argv ) {
    long terminos;
    int hilos;
    double start, finish, wused, piCritical, piReduction;

    terminos = 1000000;
    if(argc > 1) {
        terminos = atol( argv[ 1 ] );
    }

    hilos = 4;
    if( argc > 2 ) {
        hilos = atoi( argv[ 2 ] );
    }

    printf( "\tValor de PI: %.20g (de la biblioteca de matemáticas)\n", M_PI );
    printf( "Calculando para %ld términos\n", terminos );

    start = omp_get_wtime();
    serialTest( terminos );
    finish = omp_get_wtime();
    wused = finish - start;
    printf( "Serial wall time: %.6f s\n", wused );

    printf( "\nCalculando con %d hilos con (CRITICAL)\n", hilos );
    start = omp_get_wtime();
    piCritical = parallelPiCritical( terminos, hilos );
    finish = omp_get_wtime();
    printf( "OpenMP Critical PI:  \033[91m%.20g\033[0m\n", piCritical );
    printf( "OpenMP Critical wall time: %.6f s\n", wused );

    printf( "\nCalculando con %d hilos con (REDUCTION)\n", hilos );
    start = omp_get_wtime();
    piReduction = parallelPiReduction( terminos, hilos );
    finish = omp_get_wtime();
    printf( "OpenMP Reduction PI:  \033[91m%.20g\033[0m\n", piReduction );
    printf( "OpenMP Reduction wall time: %.6f s\n", wused );

}
