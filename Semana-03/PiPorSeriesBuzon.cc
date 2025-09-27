/**
  * Calcula el numero PI utilizando una serie "infinita"
  * Debe recibir la cantidad de iteraciones como parametro
  *
  *   pi = (-1)^i x 4/(2xi + 1)
  *
  *  Autor: Programacion Paralela y Concurrente (Francisco Arroyo)
  *  Fecha: 2025/Ago/21
  *
 **/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "Buzon.h"

Buzon m;

/*
   Realiza la acumulacion de terminos desde una posicion inicial hasta un termino final
*/
double calcularSumaParcialPi( long inicial, long fin ) {
   double casiPi = 0;
   // double alterna = 4;
   long divisor = 0;
   long termino;

   double alterna = (inicial % 2 == 0) ? 4.0 : -4.0;
   
   for ( termino = inicial; termino < fin; termino++ ) {
      divisor = termino + termino + 1;		// 2 x termino + 1
      casiPi += alterna/divisor;		// 4 / (2xi + 1)
      alterna *= -1;				// Pasa de 4 a -4 y viceversa, para realizar la aproximacion de los terminos
   }

   // ========== recordar el empaquetado y desempaquetado si mi formato no es exactamente igual ==========
   Mensaje msg;
   msg.mtype = 1;
   snprintf(msg.label, LABEL_SIZE, "Proceso %ld", inicial);
   memcpy(&msg.times, &casiPi, sizeof(double));
   m.Enviar(&msg, sizeof(msg) - sizeof(long), 1);

   // Pi[ proceso ] = casiPi;			// Guarda el resultado en el vector y finaliza

   exit( 0 );
}


int main( int argc, char ** argv ) {
   long terminos, inicio, fin;
   int proceso;
   int pid;
   // double casiPi[ 10 ] = { 0 };

   terminos = 1000000;
   if ( argc > 1 ) {
      terminos = atol( argv[ 1 ] );
   }

   for ( proceso = 0; proceso < 10; proceso++ ) {
      inicio = proceso * terminos/10;
      fin = (proceso + 1) * terminos/10;
      pid = fork();
      if ( pid == 0 ) {
         calcularSumaParcialPi( inicio, fin );
      } else {
         printf("Creating process %d: starting value %ld, finish at %ld\n", pid, inicio, fin );
      }
   }

   for ( proceso = 0; proceso < 10; proceso++ ) {
      int status;
      wait( &status );
   }

   // for ( proceso = 1; proceso < 10; proceso++ ) {
   //    casiPi[ 0 ] += casiPi[ proceso ];
   // }

   // ========== desempaquetado ==========
   double pi = 0;
   for (int i = 0; i < 10; i++) {
      Mensaje msg;
      m.Recibir(&msg, sizeof(msg) - sizeof(long), 1);
      double parcial;
      memcpy(&parcial, &msg.times, sizeof(double));
      pi += parcial;
   }

   printf( "Valor calculado de Pi es \033[91m %g \033[0m con %ld terminos\n", pi, terminos );

   return 0;
}
