/**
 *  C++ class to encapsulate Unix semaphore intrinsic structures and system calls
 *  Author: Programacion Concurrente (Francisco Arroyo)
 *  Version: 2025/Ago/18
 *
 * Ref.: https://en.wikipedia.org/wiki/Semaphore_(programming)
 *
 **/

#include <stdexcept>            // runtime_error
#include <sys/sem.h>
#include "Semaforo.h"

#define KEY 0xb74200
   
/**
  * Class constructor
  *    Build a operating system semaphore set, using semget
  *    Could provide a initial value using semctl, if not it will be zero
  *
 **/
Semaforo::Semaforo( int valorInicial ) {
   int resultado = -1;

   union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
           } k ;

// Use semget to create a System V semaphore set identifier

   resultado = semget(KEY, 1, IPC_CREAT | 0600);

// Use semctl to state a initial value to semaphore set
   if ( -1 == resultado ) {
      throw std::runtime_error( "Semaforo::Semaforo( int )" );
   }

   this->id = resultado;
   k.val = valorInicial;
   resultado = semctl(this->id, 0, SETVAL, k);

   if ( -1 == resultado ) {
      throw std::runtime_error( "Semaforo::Semaforo( int )" );
   }

}

/**
  * Class destructor
  *    Destroy a semaphore set identifier, using semctl
  *
 **/
Semaforo::~Semaforo() {
// Use semctl to destroy a semaphore set identifier
   int resultado;
   resultado = semctl(this->id, 0, IPC_RMID);
}

/**
  * Signal method 
  *    Add 1 to sempahore value, check if are waiting process and awake first one
  *
 **/
int Semaforo::Signal() {
   int resultado = -1;

// Use semop

   struct sembuf s;
   {
      s.sem_num = 0;
      s.sem_op = +1;
      s.sem_flg = 0;
   };
   
   resultado = semop(this->id, &s, 1);

   if ( -1 == resultado ) {
      throw std::runtime_error( "Semaforo::Signal()" );
   }

   return resultado;

}

/**
  * Wait method 
  *    Substract 1 to sempahore value, check if negative or zero, calling process will sleep
  *    and wait for next Signal operation
  *
 **/
int Semaforo::Wait() {
   int resultado = -1;
   // struct sembuf w; // Uso la declaracion de abajo

// Use semop

   struct sembuf w;
   {
      w.sem_num = 0;
      w.sem_op = -1;
      w.sem_flg = 0;
   };

   resultado = semop(this->id, &w, 1);

   if ( -1 == resultado ) {
      throw std::runtime_error( "Semaforo::Wait()" );
   }

   return resultado;
}
