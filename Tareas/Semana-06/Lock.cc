/**
  * PThreads
  *
  *  Esta clase encapsula las funciones para la utilizacion de Locks
  *
  *  Autor: Programacion Paralela y Concurrente
  *
  *  Fecha: 2025/Set/16
  *
 **/

#include "Lock.h"

/*
 *  Creates a lock, using a mutex
 */
Lock::Lock() {

  this->mutex = new Mutex();

}


/**
 * Destroys the lock
 */
Lock::~Lock() {

  delete this->mutex;

}


/**
 * Acquires the lock
 *
 */
void Lock::Acquire() {

  this->mutex->Lock();

} 

  
/**
 * Release the lock
 *
 */
void Lock::Release() {

  this->mutex->Unlock();

}

Mutex * Lock::getMutex() {
  return this->mutex;
}
