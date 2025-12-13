/**
  *  C++ class to encapsulate Unix semaphore intrinsic structures and system calls
  *
  *  Author: Programacion Concurrente (Francisco Arroyo)
  *
  *  Version: 2025/Oct/21
  *
  *  En esta nueva versión vamos a construir semáforos con OpenMP
  *
 **/

#include <stdexcept>            // runtime_error

#include "Semaphore.h"

/**
  * Class constructor
  *    Build a operating system semaphore set, using semget
  *    Could provide a initial value using semctl, if not it will be zero
  *
  *
  *    Init lock variable, set initial value
  *
 **/
Semaphore::Semaphore( int valorInicial ) {
   int resultado = -1;

   // reservo memoria para los locks
    mutex = new omp_lock_t;
    sem = new omp_lock_t;

    // inicializo los locks
    omp_init_lock( mutex );
    omp_init_lock( sem );

    // valor incial del semaforo
    value = valorInicial;
    owner = -1; // nadie tiene le recurso al inicio

    // si el semaforo inicia en 0 queda bloqueado hasta que reciba un signal
    if( value <= 0 ) {

        omp_set_lock( sem );
    }

    resultado = 0;

   if ( -1 == resultado ) {
      throw std::runtime_error( "Semaphore::Semaphore( int )" );
   }


}


/**
  * Class destructor
  *    Destroy a semaphore set identifier, using semctl
  *
 **/
Semaphore::~Semaphore() {

    // destruyo los locks
    omp_destroy_lock( mutex );
    omp_destroy_lock( sem );

    // libero memoria reservada para los locks
    delete mutex;
    delete sem;
}


/**
  * Signal method 
  *    Add 1 to sempahore value, check if are waiting process and awake first one
  *
 **/
int Semaphore::Signal() {

   omp_set_lock( this->mutex );
   this->value++;
   omp_unset_lock( this->mutex );
   omp_unset_lock( this->sem );

   return 0;

}


/**
  * Wait method 
  *    Substract 1 to sempahore value, check if negative or zero,
  *    calling process will sleep and wait for next Signal operation
  *
 **/
int Semaphore::Wait() {

   while ( 1 ) {
      omp_set_lock( this->mutex );
      if ( this->value > 0 ) {
         this->value--;
         omp_unset_lock( this->mutex );
         break;
      } else {
         omp_unset_lock( this->mutex );
         omp_set_lock( this->sem );
      }
   }

   return 0;

}
