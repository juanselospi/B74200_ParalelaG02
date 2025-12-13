/**  Esta clase encapsula las funciones para la utilizacion de Locks
  *
  *  Author: Programacion Concurrente (Francisco Arroyo)
  *
  *  Version: 2025/Ago/29
 **/

#include "Lock.h"

/*
 *  Creates a lock, using a semaphore
 */
Lock::Lock() {

   this->lock = new Semaforo( 1, 1 );

}


/**
 * Destroys the lock
 */
Lock::~Lock() {

   delete this->lock;

}


/**
 * Acquires the lock
 *
 */
void Lock::Acquire() {

   this->lock->Wait(0);

} 

  
/**
 * Release the lock
 *
 */
void Lock::Release() {

   this->lock->Signal(0);

}

