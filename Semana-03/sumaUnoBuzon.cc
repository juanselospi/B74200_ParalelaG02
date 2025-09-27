/**
  * Suma uno a un total mil veces por cada proceso generado
  * Recibe como parametro la cantidad de procesos que se quiere arrancar
  *
  * Author: Programacion Concurrente (Francisco Arroyo)
  *
  * Version: 2020/Ago/08
 **/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "Buzon.h"

long total = 0;

Buzon m;

/*
 *  Do some work, by now add one to a variable
 */
void AddOne() {
   int i;
   long suma = 0;

   for ( i = 0; i < 1000; i++ ) {
      usleep( 1 );
      suma++;			// Suma uno
   }

   m.Enviar("SUMA", suma, 1);

   exit( 0 );

}

/*
  Serial test
*/
int SerialTest( int procesos, long * total ) {
   int i, proceso;

   for ( proceso = 0; proceso < procesos; proceso++ ) {

      for ( i = 0; i < 1000; i++ ) {
         (* total)++;			// Suma uno
      }

   }
   return 0;
}

/*
  Fork test
*/
int ForkTest( int procesos, long * total ) {
   int proceso, pid;

   for ( proceso = 0; proceso < procesos; proceso++ ) {
      pid = fork();
      if ( pid == 0 ) {
         AddOne();
      }
   }

   for ( proceso = 0; proceso < procesos; proceso++ ) {
      int status;
      wait( &status );
   }


   for ( proceso = 0; proceso < procesos; proceso++) {
      struct Mensaje msg;
      m.Recibir(&msg, sizeof(msg) - sizeof(long), 1);
      (* total) += msg.times;
   }

   return 0;
}

/*
 *
 */
void startTimer( struct timeval * timerStart) {
   gettimeofday( timerStart, NULL );
}

/*
 *  time elapsed in ms
 */
double getTimer( struct timeval timerStart ) {
   struct timeval timerStop, timerElapsed;

   gettimeofday(&timerStop, NULL);
   timersub(&timerStop, &timerStart, &timerElapsed);
   return timerElapsed.tv_sec*1000.0+timerElapsed.tv_usec/1000.0;
}

int main( int argc, char ** argv ) {
   long procesos;
   int proceso;
   struct timeval timerStart;
   double used;



   procesos = 100;
   if ( argc > 1 ) {
      procesos = atol( argv[ 1 ] );
   }

   startTimer( & timerStart );
   total = 0;
   SerialTest( procesos, & total );
   used = getTimer( timerStart );
   printf( "Serial version:      Valor acumulado por %ld procesos es \033[91m %ld \033[0m en %f ms\n", procesos, total, used );

   startTimer( & timerStart );
   total = 0;
   ForkTest( procesos, & total );
   used = getTimer( timerStart );
   printf( "Fork   version:      Valor acumulado por %ld procesos es \033[91m %ld \033[0m en %f ms\n", procesos, total, used );

   return 0;
}
