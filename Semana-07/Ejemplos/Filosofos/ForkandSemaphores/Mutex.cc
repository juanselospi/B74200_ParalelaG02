/*  Esta clase encapsula las funciones para la utilizacion de Mutex
 *
 *  Autor: Programacion Paralela y Concurrente
 *  Fecha: 2020/May/16
 *
 */

#include "Mutex.h"

/*
 *  Creates a mutex, using a semaphore
 */
Mutex::Mutex() {

   this->mutex = new Semaphore( 1 );

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

