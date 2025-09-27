/**
  *
  *  PThread (base) example for resolving water problem
  *
  *  Author: Programacion Concurrente (Francisco Arroyo)
  *
  *  Version: 2025/Ago/29
  *
 **/

#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "Semaforo.h"

Semaforo sO( 0 );	// Semaforo inicializado en 0


void * Hidrogeno( void * rank ) {
   long hilo = (long) rank;


}


void * Oxigeno( void * rank ) {
}


int main( int argc, char ** argv ) {
   long hilos, hilo;
   pthread_t * pthilos;



   hilos = 100;
   if ( argc > 1 ) {
      hilos = atol( argv[ 1 ] );
   }

   pthilos = (pthread_t *) calloc( hilos, sizeof( pthread_t ) );
   srand( getpid() );
   for ( hilo = 0; hilo < hilos; hilo++ ) {
      if ( random() % 2 ) {
         pthread_create( & pthilos[ hilo ], NULL, Hidrogeno, (void *) hilo );
      } else {
         pthread_create( & pthilos[ hilo ], NULL, Oxigeno, (void *) hilo );
      }
   }

   for ( hilo = 0; hilo < hilos; hilo++ ) {
      pthread_join( pthilos[ hilo ], NULL );
   }

   free( pthilos );

}

