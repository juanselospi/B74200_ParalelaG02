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
#include <sys/shm.h>

// # ==============================
// # Este es un ejemplo de manipular fork() sin necesidad de semaforo
// # ==============================

/*
   Realiza la acumulacion de terminos desde una posicion inicial hasta un termino final
*/
double calcularSumaParcialPi( double Pi[], int proceso, long inicial, long terminos ) {
   double casiPi = 0;
   double alterna = 4;
   long divisor = 0;
   long termino;

   for ( termino = inicial; termino < terminos; termino++ ) {
      divisor = termino + termino + 1;		// 2 x termino + 1
      casiPi += alterna/divisor;		// 4 / (2xi + 1)
      alterna *= -1;				// Pasa de 4 a -4 y viceversa, para realizar la aproximacion de los terminos
   }
   Pi[ proceso ] = casiPi;			// Guarda el resultado en el vector y finaliza

   exit( 0 );
}


int main( int argc, char ** argv ) {
   long terminos, inicio, fin;
   int proceso;
   int pid;
   // double casiPi[ 10 ] = { 0 };
   double *casiPi;

   // segmento de memoria compartida
   int sharedMemo = shmget( 12345, 10 * sizeof( double ), IPC_CREAT | 0666 ); // importante reservar espacio para x10 procesos Pi[10]
   casiPi = ( double * ) shmat( sharedMemo, NULL, 0 );

   for( int i = 0; i < 10; i++ ) {
      casiPi[i] = 0;
   }

   terminos = 1000000;
   if ( argc > 1 ) {
      terminos = atol( argv[ 1 ] );
   }

   for ( proceso = 0; proceso < 10; proceso++ ) {
      inicio = proceso * terminos/10;
      fin = (proceso + 1) * terminos/10;
      pid = fork();
      if ( ! pid ) {
         calcularSumaParcialPi( casiPi, proceso, inicio, fin ); // no es necesario usar semaforo aqui, cada proceso escribe en su indice
      } else {
         printf("Creating process %d: starting value %ld, finish at %ld\n", pid, inicio, fin );
      }
   }

   for ( proceso = 0; proceso < 10; proceso++ ) {
      int status;
      pid_t pid = wait( &status );
   }

   for ( proceso = 1; proceso < 10; proceso++ ) {
      casiPi[ 0 ] += casiPi[ proceso ];
   }

   printf( "Valor calculado de Pi es \033[91m %g \033[0m con %ld terminos\n", casiPi[ 0 ], terminos );

   shmdt( casiPi );
   shmctl( sharedMemo, IPC_RMID, NULL );
}

