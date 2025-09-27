/**
  *  C++ program to test operating system shared memory segments
  *
  *  No C++ class implementation
  *
  *  Author: Programacion Concurrente (Francisco Arroyo)
  *
  *  Version: 2025/Ago/21
  *
 **/

#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>

//#include "Semaphore.h"
#include "Semaforo.h"

int main() {

   int i;
   //Semaphore s;
   Semaforo s;
   
   int id;
   char * area, *var;
   char newvar[100];

   id = shmget( 123456, 1024, 0700 | IPC_CREAT );
   area = (char *) shmat( id, NULL, 0 );
   var = (char *) "Area de memoria compartida";

   if ( fork() ) {
      strcpy( area, var );	// El padre pasa la informacion al area compartida
      printf( "Escriba un numero y  para continuar ...\n" );
      scanf( "%i", &i );
      printf( "Padre-> valor de la variable compartida es %s \n", area );
      s.Signal();		// Avisa al hijo que la memoria tiene la informacion
   }
   else {
      s.Wait();			// Espera hasta que el padre lo indique
      strcpy( newvar, area );	// Copia y despliega la nueva variable
      printf( "Hijo->  valor de la variable compartida es %s \n", newvar );
      _exit( 0 );		// Termina al hijo sin llamar a los destructores
   }
   wait( &i );			// El padre espera a que el hijo finalice
   printf( "Fin del programa ... \n" );
   shmdt( area );		// Se "despega" del area compartida
   shmctl( id, IPC_RMID, NULL );	// Destruye el semgmento de memoria compartida
}

/*
   Ejemplo de salida

Escriba un numero y  para continuar ...
6
Padre-> valor de la variable compartida es: Area de memoria compartida 
Hijo->  valor de la variable compartida es: Area de memoria compartida 
Fin del programa ... 

*/
