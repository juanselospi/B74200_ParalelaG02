/**
 *   OpenMP semaphore example
 *
 *   Program exit depends on semaphore initial value (v)
 *     v > 0, L21, L23, L18
 *     v <= 0 L21, L18, L23
**/

#include <cstdio>
#include <unistd.h>
#include "Semaphore.h"

int main( int cantidad, char ** valores ) {

   Semaphore s( 0 );
#pragma omp parallel num_threads( 2 )
   if ( omp_get_thread_num() & 1 ) {
      sleep( 2 );
      printf( "L18, Hilo %d realiza Signal\n", omp_get_thread_num() );
      s.Signal();	// omp_unset_lock ++
   } else {
      printf( "L21, Hilo %d está esperando\n", omp_get_thread_num() );
      s.Wait();	// omp_set_lock --
      printf( "L23, Hilo %d termino la espera\n", omp_get_thread_num() );
   }

}
