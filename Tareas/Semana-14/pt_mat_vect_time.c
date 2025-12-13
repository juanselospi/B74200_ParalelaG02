#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct {

    double *A;
    double *x;
    double *y;

    int m;
    int n;

    int iniFila;
    int finFila;

} hiloData;

double wtime() {
    struct timeval tv;
    gettimeofday( &tv, 0 );
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

void *multiplicar( void *arg ) {

    hiloData *data = ( hiloData * ) arg; // void puntero a mi struct

    double *A = data->A;
    double *x = data->x;
    double *y = data->y;

    int m = data->m;
    int n = data->n;

    int ini = data->iniFila;
    int fin = data->finFila;

    int i, j;

    for( i = ini; i < fin; i++ ) {

        y[ i ] = 0.0;

        for( j = 0; j < n; j++ ) {

            y[ i ] += A[ i * n + j ] * x[ j ];
        }
    }

    return 0;
}

int main( int argc, char *argv[] ) {

    if( argc != 4 ) {

        printf("Para correr el progama: %s [hilos] [rows] [columns]\n", argv[ 0 ] );
        return 1;
    }

    int hilos = atoi( argv[ 1 ] );
    int m = atoi(argv[ 2 ] );
    int n = atoi(argv[ 3 ] );

    double *A = malloc( m * n * sizeof( double ));
    double *x = malloc (n * sizeof( double ));
    double *y = malloc( m * sizeof( double ));

    // inicializo la matriz A
    for( int i = 0; i < m; i++ ) {

        for( int j = 0; j < n; j++ ) {

            A[ i*n + j ] = ( ( double ) rand()) / RAND_MAX;
        }
    }

    // inicializo el vector x
    for( int i = 0; i < n; i++ ) {

        x[ i ] = (( double ) rand()) / RAND_MAX;
    }

    pthread_t *thredIds = malloc( hilos * sizeof( pthread_t ) );
    hiloData *info = malloc( hilos * sizeof( hiloData ) );

    int filasPorHilo = m / hilos;
    int resto = m % hilos;

    double start = wtime();

    // creo mis hilos para emepezar
    int filaActual = 0;

    for( int t = 0; t < hilos; t++ ) {

        int inicio = filaActual;

        int cantidad;

        if( t < resto ) {

            cantidad = filasPorHilo + 1;
        } else {
            
            cantidad = filasPorHilo;
        }

        int fin = inicio + cantidad;

        info[ t ].A = A;
        info[ t ].x = x;
        info[ t ].y = y;
        info[ t ].m = m;
        info[ t ].n = n;
        info[ t ].iniFila = inicio;
        info[ t ].finFila = fin;

        pthread_create( &thredIds[ t ], 0, multiplicar, &info[ t ] );

        filaActual = fin;
    }

    // espero a que los hilos terminen y hago el jjoin
    for( int t = 0; t < hilos; t++ ) {

        pthread_join( thredIds[ t ], 0 );
    }

    double finish = wtime();

    printf( "Tiempo de ejecucion con Pthreads para %d hilos = %e\n", hilos, finish - start );

    free( A );
    free( x );
    free( y );
    free( thredIds );
    free( info );

    return 0;
}
