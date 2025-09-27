/**  Esta clase encapsula las funciones para la utilizacion de Mutex
  *
  *  Author: Programacion Concurrente (Francisco Arroyo)
  *
  *  Version: 2025/Ago/29
 **/

#include "Mutex.h"

/*
 *  Creates a mutex, using a semaphore
 */
Mutex::Mutex() {

   this->mutex = new Semaforo( 1, 1 );

}


/**
 * Destroys the lock
 */
Mutex::~Mutex() {

   delete this->mutex;

}


/**
 * Acquires the lock
 *
 */
void Mutex::Lock() {

   this->mutex->Wait();

} 

  
/**
 * Release the lock
 *
 */
void Mutex::Unlock() {

   this->mutex->Signal();

}
