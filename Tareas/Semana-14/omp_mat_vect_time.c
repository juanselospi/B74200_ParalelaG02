#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

double wtime() {
    struct timeval tv;
    gettimeofday( &tv, 0 );
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

int main( int argc, char *argv[] ) {
    int m, n;
    int hilos;
    double *A, *x, *y;
    double start, finish;
    int i, j;

    if( argc != 4 ) {

        // parametros para correr la funcion por si me olvido
        printf( "Para correr el programa: %s [hilos] [rows] [columns]\n", argv[ 0 ] );
        return 1;
    }

    hilos = atoi( argv[ 1 ] );
    m = atoi( argv[ 2 ] );
    n = atoi( argv[ 3 ] );

    A = malloc( m * n * sizeof( double ) );
    x = malloc( n * sizeof( double ) );
    y = malloc( m * sizeof( double ) );

    // inicializo la matriz A usando alores aleatorios
    for( i = 0; i < m; i++ ) {

        for( j = 0; j < n; j++ ) {

            A[ i*n + j ] = ( ( double ) rand() ) / RAND_MAX;
        }
    }

    // inicializo mi vector x 
    for( i = 0; i < n; i++ ) {

        x[ i ] = ( ( double ) rand() ) / RAND_MAX;
    }

    start = wtime();

    // producto de la matriz y vector con OpenMP
    #pragma omp parallel for num_threads( hilos ) private( j )
    for( i = 0; i < m; i++ ) {

        y[ i ] = 0.0;

        for( j = 0; j < n; j++ ) {

            y[ i ] += A[ i*n + j ] * x[ j ];
        }
    }

    finish = wtime();

    printf( "Tiempo de ejecucion en OpenMP (%d hilos) = %e\n", hilos, finish - start );

    free( A );
    free( x );
    free( y );

    return 0;
}
