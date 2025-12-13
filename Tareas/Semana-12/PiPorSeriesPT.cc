/**
  * Calcula el numero PI utilizando una serie "infinita"
  * Debe recibir la cantidad de iteraciones como parametro
  *
  *   pi = (-1)^i x 4/(2xi + 1)
  *
  *  Autor: Programacion Paralela y Concurrente (Francisco Arroyo)
  *
  *  Fecha: 2025/Oct/21
  *
 **/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <thread>
#include <vector>
#include <mutex>
#include <math.h>


/**
  * Global variables
 **/
long * inicio;
long iteraciones = 0;
double pi = 0;
std::mutex mutex;


/**
  *
 **/
void startTimer( struct timeval * timerStart) {
   gettimeofday( timerStart, NULL );
}


/**
  *  time elapsed in ms
 **/
double getTimer( struct timeval timerStart ) {
   struct timeval timerStop, timerElapsed;

   gettimeofday(&timerStop, NULL);
   timersub(&timerStop, &timerStart, &timerElapsed);
   return timerElapsed.tv_sec*1000.0+timerElapsed.tv_usec/1000.0;
}


/**
  *  Realiza la acumulacion de terminos desde una posicion inicial,
  *  una cantidad de iteraciones definida
  *  Acumula los terminos positivos y negativos por aparte, luego realiza una suma final de ambos
  *
 **/
double calcularSumaParcialPi2( int hilo, long inicial, long iteraciones ) {
   double pos, neg;
   double alterna = 4;
   long divisor = 0;
   long termino, veces;
   long inicioParaNegativos, inicioParaPositivos;

   if ( inicial % 2 ) {		// El valor inicial es impar, debe empezar con un negativo
      inicioParaNegativos = inicial;
      inicioParaPositivos = inicial + 1;
   } else {
      inicioParaNegativos = inicial + 1;
      inicioParaPositivos = inicial;
   }

   iteraciones >>= 1;
   alterna = 4;
   pos = 0;
   for ( veces = 0, termino = inicioParaPositivos; veces < iteraciones; veces++, termino+= 2 ) {
      divisor = termino + termino + 1;		// 2 x termino + 1
      pos += alterna/divisor;			// 4 / (2xi + 1)
   }

   alterna = -4;
   neg = 0;
   for ( veces = 0, termino = inicioParaNegativos; veces < iteraciones; veces++, termino+= 2 ) {
      divisor = termino + termino + 1;		// 2 x termino + 1
      neg += alterna/divisor;			// 4 / (2xi + 1)
   }

   mutex.lock();
   pi += pos;			// Acumula el resultado
   pi += neg;
   mutex.unlock();

   return pi;

}


/**
  *  Realiza la acumulacion de terminos desde una posicion inicial,
  *  una cantidad de iteraciones definida
 **/
double calcularSumaParcialPi( int hilo, long inicial, long iteraciones ) {
   double casiPi = 0;
   double alterna = 4;
   long divisor = 0;
   long termino, veces;

   if ( inicial % 2 ) {
      alterna = -4;
   }
//printf( "Hilo %ld inicia en %ld, termina en %ld \n", hilo, inicial, inicial + iteraciones - 1 );
   for ( veces = 0, termino = inicial; veces < iteraciones; veces++, termino++ ) {
      divisor = termino + termino + 1;		// 2 x termino + 1
      casiPi += alterna/divisor;		// 4 / (2xi + 1)
//printf( "CasiPi = %.20g\n", casiPi );
      alterna *= -1;				// Pasa de 4 a -4 y viceversa, para realizar la aproximacion de los terminos
   }

   mutex.lock();
   pi += casiPi;			// Acumula el resultado en la variable global
   mutex.unlock();

   return pi;

}


/**
  *
 **/
void calcularSumaParcial( long hilo ) {

   calcularSumaParcialPi( hilo, inicio[ hilo ], iteraciones );

   pthread_exit( 0 );

}


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


/*
  Fork test with NO race condition
*/
long threadTestNoRaceCondition( long terminos, long hilos ) {
   long hilo;
   std::vector< std::thread * > trabajadores;

   iteraciones = terminos / hilos;
   inicio = (long *) calloc( hilos, sizeof( long ) );

   for ( hilo = 0; hilo < hilos; hilo++ ) {
      inicio[ hilo ] = hilo * iteraciones;
//printf( "Intervalo hilo[ %ld] = [ %ld, %ld]\n", hilo,  inicio[ hilo ], inicio[ hilo ] + iteraciones );
   }

   for ( hilo = 0; hilo < hilos; hilo++ ) {
      std::thread * nuevo  = new std::thread( calcularSumaParcial, hilo );
      trabajadores.push_back( nuevo );
   }

   for ( auto h: trabajadores ) {
      h->join();
   }

   free( inicio );

   printf( "No race cond. PI:   \033[91m %.20g \033[0m ", pi );

   return 0;

}


int main( int argc, char ** argv ) {
   long terminos, inicio, fin;
   long hilos;
   clock_t start, finish;
   struct timeval timerStart;
   double used, wused;

   hilos = 100;
   terminos = 1000000;
   if ( argc > 1 ) {
      terminos = atol( argv[ 1 ] );
   }
   if ( argc > 2 ) {
      hilos = atol( argv[ 2 ] );
   }

   printf( "\tValor de PI: %.20g (de la biblioteca de matemáticas)\n", M_PI );
   printf( "Calculando para %ld términos\n", terminos );
   startTimer( & timerStart );
   serialTest( terminos );
   wused = getTimer( timerStart );
   printf( "wall time %g \n", wused );

   printf( "Calculando para %ld hilos\n", hilos );
   startTimer( & timerStart );
   threadTestNoRaceCondition( terminos, hilos );
   wused = getTimer( timerStart );
   printf( "wall time %g \n", wused );

}
