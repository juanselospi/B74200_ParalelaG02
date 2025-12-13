#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <vector>
#include <iostream>

// Shared variables
int primo1;
int primo2;
bool found;
pthread_mutex_t * mutex;

void * primosWithMutex( void * param ) {
    std::vector<int> sumaPrimos = *( std::vector<int*> )param; 

    pthread_mutex_lock( mutex );
    found = false;
    primo1 = 3;
    primo2 = 3;
    pthread_mutex_unlock( mutex );


    for( int i = 0; i < sumaPrimos.size(); i++ ) {
        
        int temp; // para almacenar el resultado temporal

        while( found == false ) {

            if( temp == primo1 + primo2 ) {
                found = true;

            } else if( primo1 < sumaPrimos[ i ] - 2) {
                
                pthread_mutex_lock( mutex );
                primo1 + 2;
                pthread_mutex_unlock( mutex );

            } else {
                pthread_mutex_lock( mutex );
                primo2 + 2;
                pthread_mutex_unlock( mutex );

            }
        }

        std::cout << "Primos de: " << i << " = " << primo1 << " + " << primo2 << std::endl;

        pthread_mutex_lock( mutex );
        found = false;
        primo1 = 3;
        primo2 = 3;
        pthread_mutex_unlock( mutex );

    }

    pthread_exit( 0 );

}

/*
    Calcula los 2 valores que pueden componer la entrada del conjunto de enteros
*/
int calcularPrimosEntrada( std::vector<int>sumaPrimos, long hilos ) {
    long hilo;
    pthread_t * pthilos;

    mutex = (pthread_mutex_t *) calloc( 1, sizeof( pthread_mutex_t ) );
    pthread_mutex_init( mutex, NULL );
    pthilos = (pthread_t *) calloc( hilos, sizeof( pthread_t ) );

    for ( hilo = 0; hilo < hilos; hilo++ ) {
      pthread_create( & pthilos[ hilo ], NULL, primosWithMutex, (void *) hilo);
    }

}

/*
    Serial test
*/
void SerialTest( std::vector<int>sumaPrimos ) {
    int primo1 = 3;
    int primo2 = 3;

    bool found = false;

    for( int i = 0; i < sumaPrimos.size(); i++ ) {
        
        int temp; // para almacenar el resultado temporal

        while( found == false ) {

            if( temp == primo1 + primo2 ) {
                found = true;

            } else if( primo1 < sumaPrimos[ i ] - 2) {
                primo1 + 2;

            } else {
                primo2 + 2;
            }
        }

        std::cout << "Primos de: " << i << " = " << primo1 << " + " << primo2 << std::endl;

    }

}

/*
    Timer starter
 */
void startTimer( struct timeval * timerStart) {
   gettimeofday( timerStart, NULL );
}

/*
 *  Time elapsed in ms
 */
double getTimer( struct timeval timerStart ) {
   struct timeval timerStop, timerElapsed;

   gettimeofday(&timerStop, NULL);
   timersub(&timerStop, &timerStart, &timerElapsed);
   return timerElapsed.tv_sec*1000.0+timerElapsed.tv_usec/1000.0;
}



int main( int argc, char ** argv ) {
    long hilos;
    clock_t start, finish;
    struct timeval timerStart;
    double used, wused;

    total = 0; // borrar luego

    int numMayor;

    std::vector<int> sumaPrimos;


    hilos = 100;
    if ( argc > 1 ) {
        hilos = atol( argv[ 1 ] );
    }

    numMayor = 6;
    if ( argc > 2 ) {
        numMayor = atol( argv[ 2 ] );
    }
    
    for( int i = 6; i <= numMayor; i + 2 ) {
        sumaPrimos.push_back( i );
    }
 

    // calculo de forma serial
    startTimer( &timerStart );
    start = clock();
    total = 0;
    SerialTest ( sumaPrimos );
    finish = clock();
    used = ((double) (finish - start)) / CLOCKS_PER_SEC;
    wused = getTimer( timerStart );
    printf( "Serial version:      total con %ld hilos, CPU time %g seconds, wall time %g \n", hilos, used, wused );

    //calculo por pthreads distribuyendo tareas
    startTimer( &timerStart );
    start = clock();
    total = 0;
    calcularPrimosEntrada ( sumaPrimos, hilos );
    finish = clock();
    used = ((double) (finish - start)) / CLOCKS_PER_SEC;
    wused = getTimer( timerStart );
    printf( "Pthread version:      total con %ld hilos, CPU time %g seconds, wall time %g \n", hilos, used, wused );

}

// para correrlo los parametro de cuantos hilos se usan y el numero n se agrega en la ejecucion
// usar: make bin/preguntaA.cc
// luego: ./bin/preguntaA [h] [n]
// donde: [h] es el numero de hilos y [n] el numero n par