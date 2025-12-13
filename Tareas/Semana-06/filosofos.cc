/**
  * Resuelve el problema de los filósofos utilizando un monitor "Mesa"
  * 
  * Author: Programacion Concurrente (Francisco Arroyo)
  *
  * Version: 2025/Set/16
 **/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#include <pthread.h>    // para usar hilos
#include <vector>   // el tamaño de los arrays debe ser constante en tiempo de compilacion, vector me permite usar arrays dinamicos mas seguros
#include <random>   // como los hilos pertenecen al mismo proceso necesito otra forma de dar valores aleatorios porque su getpid() seria el mismo numero

#include "Mesa.h"
#include "Barrier.h"

struct FiloMesa {   // estructura pasar pasar argumentos con pthread
   Mesa * redonda;  // puntero a la mesa compartida
   int filoId;
   Barrier * barrera;
};

/*
 *  Philosophers code adapted for pthread
 */
void * Filosofo( void * arg ) { // pthread requiere void * return y void * arg(puntero que apunta a la estructura FiloMesa)
   FiloMesa * fArgs = ( FiloMesa * ) arg;   // convierto el void puntero a mi struct
   
   // extraigo la informacion
   int cual = fArgs -> filoId;
   Mesa * mesa = fArgs -> redonda;
   Barrier * barrera = fArgs -> barrera;
    
   int i;
   int eat, think;

   // no es confiable con hilos
//    srand( getpid() );

   barrera->Wait();

   std::random_device rd;
   std::mt19937 gen(rd() + cual);
   std::uniform_int_distribution<> dist(0, 0xfffff);


   for ( i = 0; i < 10; i++ ) {	// Do a round for thinking and eating
      think = dist(gen);
      usleep( think );
      mesa->pickup( cual );
      printf( "El filósofo %d está comiendo\n", cual );
      eat = dist(gen);
      usleep( eat );
      mesa->putdown( cual );
      printf( "El filósofo %d está pensando\n", cual );
      think = dist(gen);
      usleep( think );
   }

   return nullptr;  // en pthread no se usa exit

}

/*
 *
 */
int main( int argc, char ** argv ) {
   long workers;
   int worker;

   workers = 5;
   if ( argc > 1 ) {
      workers = atol( argv[ 1 ] );  // toma el primer argumento que pase al programa -> atol(argv[1]), convierte ese texto en un numero, asigna el resultado a la variable workers, ./filosofos 10 vuelve workers = 10, si no lo uso por defecto workers = 5
   }

   Mesa mesa;   // instancia de objeto mesa en el main stack
   std::vector<pthread_t> hilos(workers);  // arreglo de hilos 1 hilo = 1 filosofo
   std::vector<FiloMesa> args(workers);  // arreglo de argumentos para cada hilo
   Barrier barrera(workers);

   // crear hilos filosofos
   for(worker = 0; worker < workers; ++worker) {
      args[worker].filoId = worker;
      args[worker].redonda = &mesa;
      args[worker].barrera = &barrera;

      pthread_create(&hilos[worker], NULL, Filosofo, &args[worker]);
   }

   // espera a que los filosofos terminen
   for ( worker = 0; worker < workers; worker++ ) {
      pthread_join(hilos[worker], NULL);
   }

   // pthread maneja la destruccion de los hilos creados al terminar
//    shmdt( filoMesa );
//    shmctl( memId, IPC_RMID, NULL );

   return 0;

}
