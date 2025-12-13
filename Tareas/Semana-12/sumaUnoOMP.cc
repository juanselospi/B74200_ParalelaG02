/**
 *  Suma uno a un total mil veces por cada hilo generado (version OpenMP)
 *  Compilar con:
 *     make sumaUnoOMP.out
 *  Ejecutar con:
 *     ./sumaUnoOMP.out [num_hilos]
 *  [DEBUG]: Compilar con: 
 *     g++ -fopenmp -DDEBUG sumaUnoOMP.cc -o sumaUnoOMP.out
 *  [DEBUG]: Ejecutar con:
 *     ./sumaUnoOMP.out [num_hilos]
 */

#include <iostream>
#include <unistd.h>
#include <omp.h>

using namespace std;

long total = 0;

#ifdef DEBUG
int* iterations; // guarda que hilo ejecuta que iteracion
#endif

/*
 *  Serial Test
 */
long SerialTest( long hilos ) {
    total = 0;

    for ( long hilo = 0; hilo < hilos; hilo++ ) {
        for ( int i = 0; i < 1000; i++ ) {
            total++;
            usleep( 1 );
        }

    }

    return total;
}

/*
 *  test paralelo con condicion de carrera
 */
long ParallelRaceCondition( long hilos ) {
    total = 0;

#ifdef DEBUG
    iterations = ( int* ) malloc( hilos * sizeof( int ) );
#endif

    #pragma omp parallel for num_threads( hilos ) shared( total )
    for( long hilo = 0; hilo < hilos; hilo++ ) {

        for( int i = 0; i < 1000; i++ ) {

            total++;
            usleep( 1 );
        }

#ifdef DEBUG
        iterations[ hilo ] = omp_get_thread_num();
#endif

    }

#ifdef DEBUG
    printf( "\n[DEBUG] Asignación de iteraciones (Race Condition):\n" );
    printf( "Thread\t\tIteraciones\n" );
    printf( "------\t\t----------\n" );

    int n = 1000;
    int thread_count = hilos;

    for( int t = 0; t < thread_count; t++ ) {

        int base = n / thread_count;
        int extra = n % thread_count;

        int inicio, fin;

        if( t < extra ) {

            inicio = t * ( base + 1 );
            fin = inicio + base;
        } else {

            inicio = t * base + extra;
            fin = inicio + base - 1;
        }

        printf( "  %2d\tIteraciones %4d a %4d\n", t, inicio, fin );
    }

    free( iterations );
#endif

    return total;
}

/*
 *  test paralelo sin condicion de carrera usando seccion critica
 */
long ParallelNoRaceCondition( long hilos ) {
    total = 0;

#ifdef DEBUG
    iterations = ( int* ) malloc( hilos * sizeof( int ) );
#endif

    #pragma omp parallel for num_threads( hilos ) shared( total )
    for( long hilo = 0; hilo < hilos; hilo++ ) {

        long myTotal = 0;
        for( int i = 0; i < 1000; i++ ) {

            myTotal++;
            usleep( 1 );
        }

        // zona critia
        #pragma omp critical
        {
            total += myTotal;
        }

#ifdef DEBUG
        iterations[ hilo ] = omp_get_thread_num();
#endif

    }

#ifdef DEBUG
    printf( "\n[DEBUG] Asignación de iteraciones (CRITICAL):\n" );
    printf( "Thread\t\tIteraciones\n" );
    printf( "------\t\t----------\n" );

    int n = 1000;
    int thread_count = hilos;

    for( int t = 0; t < thread_count; t++ ) {

        int base = n / thread_count;
        int extra = n % thread_count;

        int inicio, fin;

        if( t < extra ) {

            inicio = t * ( base + 1 );
            fin = inicio + base;
        } else {

            inicio = t * base + extra;
            fin = inicio + base - 1;
        }

        printf( "  %2d\tIteraciones %4d a %4d\n", t, inicio, fin );
    }

    free( iterations );
#endif

    return total;
}

/*
 *  version alternativa sin condiciones de carrera usando reduccion
 */
long ParallelReduction( long hilos ) {
    total = 0;

#ifdef DEBUG
    iterations = ( int* ) malloc( hilos * sizeof( int ) );
#endif

    #pragma omp parallel for num_threads(hilos) reduction(+:total)
    for( long hilo = 0; hilo < hilos; hilo++ ) {

        for( int i = 0; i < 1000; i++ ) {

            total++;
            usleep( 1 );
        }

#ifdef DEBUG
        iterations[ hilo ] = omp_get_thread_num();
#endif

    }

#ifdef DEBUG
    printf( "\n[DEBUG] Asignación de iteraciones (REDUCTION):\n" );
    printf( "Thread\t\tIteraciones\n" );
    printf( "------\t\t----------\n" );

    int n = 1000;
    int thread_count = hilos;

    for( int t = 0; t < thread_count; t++ ) {

        int base = n / thread_count;
        int extra = n % thread_count;

        int inicio, fin;

        if( t < extra ) {

            inicio = t * ( base + 1 );
            fin = inicio + base;
        } else {

            inicio = t * base + extra;
            fin = inicio + base - 1;
        }

        printf( "  %2d\tIteraciones %4d a %4d\n", t, inicio, fin );
    }

    free( iterations );
#endif

    return total;
}

int main( int argc, char **argv ) {
    long hilos;
    double start, finish, wused;

    hilos = 10;
    if ( argc > 1 ) {
        hilos = atol( argv[ 1 ] );
    }

    // serial
    start = omp_get_wtime();
    SerialTest( hilos );
    finish = omp_get_wtime();
    wused = finish - start;
    printf( "Serial version:      total = \033[91m%ld\033[0m con wall time = %.3f s\n", total, wused );

    // OpenMP con condicion de carrera
    start = omp_get_wtime();
    ParallelRaceCondition( hilos );
    finish = omp_get_wtime();
    wused = finish - start;
    printf( "\nOpenMP Race Cond.:   total = \033[91m%ld\033[0m con %ld hilos, wall time = %.3f s\n", total, hilos, wused );

    // OpenMP sin condicion de carrera ( version seccion critica )
    start = omp_get_wtime();
    ParallelNoRaceCondition( hilos );
    finish = omp_get_wtime();
    wused = finish - start;
    printf( "\nOpenMP No Race (CRITICAL): total = \033[91m%ld\033[0m con %ld hilos, wall time = %.3f s\n", total, hilos, wused );

    // OpenMP sin condicion de carrera ( version con reduction )
    start = omp_get_wtime();
    ParallelReduction( hilos );
    finish = omp_get_wtime();
    wused = finish - start;
    printf( "\nOpenMP No Race (REDUCTION): total = \033[91m%ld\033[0m con %ld hilos, wall time = %.3f s\n", total, hilos, wused );
}
