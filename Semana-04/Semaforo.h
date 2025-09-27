/**
  *  C++ class to encapsulate Unix semaphore intrinsic structures and system calls
  *
  *  Author: Programacion Concurrente (Francisco Arroyo)
  *
  *  Version: 2025/Ago/29
  *
 **/

#include <sys/types.h>	// pid_t

#ifndef SEMAFORO
#define SEMAFORO

class Semaforo {
   public:
      Semaforo( int, int = 0 );		// semaphore quantity, initial value
      ~Semaforo();			// remove semaphore array
      int Signal( int = 0 );		// First array element operation
      int Wait( int = 0 );		// First array element operation
      void SP( int, int );		// Wait for all semaphore in array
      void SV( int, int );		// Signal all semaphores in array

   private:
      int id;		// Identificador del semaforo
      pid_t owner;	// Resource owner
      int nsems;	// Cantidad de semaforos en el arreglo
};
#endif
