/**
  *  C++ class to encapsulate Unix semaphore intrinsic structures and system calls
  *
  *  Author: Programacion Concurrente (Francisco Arroyo)
  *
  *  Version: 2025/Ago/29
  *
  *  En esta nueva versión vamos a construir un arreglo de semáforos,
  *  el tamaño es indicado por el constructor (cantidad)
  *  Las operaciones Wait y Signal reciben el parámetro sobre el cual se pretende
  *  realizar esa operación, normalmente el primero
  *  Además, vamos a agregar dos métodos nuevos para poder operar sobre dos (todos)
  *  semáforos simultáneamente (SP wait simultáneo, SV signal simultáneo)
  *   y resolver el problema de los filósofos
  *
 **/

#include <stdexcept>            // runtime_error
#include <sys/sem.h>

#include "Semaforo.h"

/**
  * Class constructor
  *    Build a operating system semaphore set, using semget
  *    Could provide a initial value using semctl, if not it will be zero
  *
 **/
Semaforo::Semaforo( int cantidad, int valorInicial ) {
   int status;
   union semun {
      int              val;
      struct semid_ds *buf;
      unsigned short  *array;
      struct seminfo  *__buf;
   } value;

   this->id = semget( IPC_PRIVATE, cantidad, 0600 | IPC_CREAT | IPC_EXCL );
   this->nsems = cantidad;

   value.val = valorInicial;	// Each semaphore will be initialized to this value
   for ( int i = 0; i < this->nsems; i++ ) {
      status = semctl( id, i, SETVAL, value );

      if ( -1 == status ) {
         throw std::runtime_error( "Semaforo::Semaforo( int, int )" );
      }

   }

}


/**
  * Class destructor
  *    Destroy a semaphore set identifier, using semctl
  *
 **/
Semaforo::~Semaforo() {

   if(semctl(id, 0, IPC_RMID) == -1) {
      perror("Semaforo::~Semaforo: IPC_RMID failed");
   }

}


/**
  * Signal method 
  *    Add 1 to sempahore value, check if are waiting process and awake first one
  *
 **/
int Semaforo::Signal( int cual ) {
   int resultado = -1;

   struct sembuf s;
   {
      /* data */
      s.sem_num = cual;
      s.sem_op = +1;
      s.sem_flg = 0;
   };
   
   resultado = semop(id, &s, 1);

   if ( -1 == resultado ) {
      throw std::runtime_error( "Semaforo::Signal( int )" );
   }
   return resultado;

}


/**
  * Wait method 
  *    Substract 1 to sempahore value, check if negative or zero, calling process will
 sleep
  *    and wait for next Signal operation
  *
 **/
int Semaforo::Wait( int cual ) {
   int resultado = -1;

   struct sembuf w;
   {
      /* data */
      w.sem_num = cual;
      w.sem_op = -1;
      w.sem_flg = 0;
   };
   
   resultado = semop(id, &w, 1);

   if ( -1 == resultado ) {
      throw std::runtime_error( "Semaforo::Wait( int )" );
   }
   return resultado;

}

/*
 * Do a Wait operation on two semaphores, will try to substract one to each semaphore
 * This operation must be atomic, not allowed to block one semaphore
 * and try to block the other
 * The block operation must occur on both semaphores atomically
 * UNIX will try to get the lock on two (all) semaphores from the group,
 * especified by parameters
 *
 */
void Semaforo::SP( int primero, int segundo ) {
   int resultado;
   struct sembuf P[ 2 ];

   P[ 0 ].sem_num = primero;
   P[ 0 ].sem_op  = -1;
   P[ 0 ].sem_flg = 0;

   P[ 1 ].sem_num = segundo;
   P[ 1 ].sem_op  = -1;
   P[ 1 ].sem_flg = 0;

   // resultado = semop( ... );
   resultado = semop(id, P, 2);

   if ( -1 == resultado ) {
      throw std::runtime_error( "Semaforo::SP( int, int )" );
   }

}


/**
  *   Add one to each semaphore in the array
  *   Awake process as necessary
 **/
void Semaforo::SV( int primero, int segundo ) {
   int resultado;

   // ...
   struct sembuf V[ 2 ];

   V[ 0 ].sem_num = primero;
   V[ 0 ].sem_op = 1;
   V[ 0 ].sem_flg = 0;

   V[ 1 ].sem_num = segundo;
   V[ 1 ].sem_op = 1;
   V[ 1 ].sem_flg = 0;

   resultado = semop(id, V, 2);

   if ( -1 == resultado ) {
      throw std::runtime_error( "Semaforo::SV( int, int )" );
   }
}

