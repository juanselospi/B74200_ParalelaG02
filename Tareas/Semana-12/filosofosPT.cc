/*
 * Resuelve el problema del agua utilizando PThreads
 * 
 * Author: Programacion Concurrente (Francisco Arroyo)
 * Version: 2022/Oct/17
 */


#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include <cstdio>

#define MaxPhilos 5
std::mutex chopsticks[ MaxPhilos ];


/**
 *  Código para cada filósofo
**/
int Filosofo( int cual ) {
   int i;
   int eat, think;
   int leftChopstick, rightChopstick;

   leftChopstick = cual;
   rightChopstick = (cual + 1) % MaxPhilos;
   srand( gettid() );
   for ( i = 0; i < 10; i++ ) {	// Do a round for thinking and eating
      think = rand() & 0xfffff;
      usleep( think );

      printf( "El filósofo %d va a tomar los palillos\n", cual );
/*  Version 1
      std::scoped_lock<std::mutex, std::mutex> come( chopsticks[ leftChopstick ], chopsticks[ rightChopstick ] );	// Picking up chopsticks (RAII)
*/
/*  Version 2
*/
      std::unique_lock<std::mutex> left( chopsticks[ leftChopstick ], std::defer_lock );
      std::unique_lock<std::mutex> right( chopsticks[ rightChopstick ], std::defer_lock );
      std::lock( left, right );
/*  Version 3
      std::lock( chopsticks[ leftChopstick], chopsticks[ rightChopstick ] );
      std::lock_guard<std::mutex> left( chopsticks[ leftChopstick ], std::adopt_lock );
      std::lock_guard<std::mutex> right( chopsticks[ rightChopstick ], std::adopt_lock );
// */
      printf( "El filósofo %d está comiendo\n", cual );
      eat = rand() & 0xfffff;
      usleep( eat );

      printf( "El filósofo %d está pensando\n", cual );
      think = rand()  & 0xfffff;
      usleep( think );
   }

   pthread_exit( 0 );

}


/*
 *
 */
int main( int count, char ** values ) {
   int workers, worker;
   std::thread * hilos;

   workers = 5;
   if ( count > 1 ) {
      workers = atol( values[ 1 ] );
   }

   hilos = (std::thread *) calloc( workers, sizeof( std::thread ) );
// Create philosophers
   for ( worker = 0; worker < workers; worker++ ) {
      srand( gettid() );
      hilos[ worker ] = std::thread ( Filosofo, worker );
   }

   for ( worker = 0; worker < workers; worker++ ) {
      hilos[ worker ].join();
   }

}
