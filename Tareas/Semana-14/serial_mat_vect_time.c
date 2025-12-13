/*
 * Versión serial de matriz por vector
 * A (m x n) * x (n) = y (m)
 *
 * Igual a el ejemplo MPI del profesor, pero sin paralelismo.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <stddef.h>

double wtime() {
    struct timeval tv;
    gettimeofday( &tv, 0 );
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

int main() {
    int m, n;
    double *A, *x, *y;
    double start, finish;
    int i, j;

    printf( "Enter number of rows\n" );
    scanf( "%d", &m );

    printf( "Enter number of columns\n" );
    scanf( "%d", &n );

    A = malloc( m * n * sizeof( double ));
    x = malloc( n * sizeof( double ));
    y = malloc( m * sizeof( double ));

    for( i = 0; i < m; i++ ) {

        for( j = 0; j < n; j++ ) {

            A[ i*n + j ] = (( double ) rand()) / RAND_MAX;
        }
    }
            
    for( i = 0; i < n; i++ ) {

        x[i] = ((double) rand()) / RAND_MAX;
    }

    start = wtime();

    for( i = 0; i < m; i++ ) {

        y[ i ] = 0.0;
        for( j = 0; j < n; j++ ) {

            y[ i ] += A[ i*n + j ] * x[ j ];
        }
    }

    finish = wtime();

    printf( "Tiempo de ejecucion en serial = %e\n", finish - start );

    free( A );
    free( x );
    free( y );

    return 0;
}
