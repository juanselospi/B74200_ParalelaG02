/**  Esta clase encapsula las funciones para la utilizacion de Locks
  *
  *  Author: Programacion Concurrente (Francisco Arroyo)
  *
  *  Version: 2025/Oct/21
 **/

#include "Lock.h"

/**
 *  Creates a lock, using a semaphore
 *
 *  omp_init_lock
 *
 **/
Lock::Lock() {

    // reservo memoria para el lock
    lock = new omp_lock_t;

    // inicializo el lock con OpenMP
    omp_init_lock( lock );
}


/**
 * Destroys the lock
 */
Lock::~Lock() {

    // libero el lock
    omp_destroy_lock( lock );

    // libero la memoria reservada para lock
    delete lock;
}


/**
 * Acquires the lock
 *
 *  omp_set_lock
 */
void Lock::Acquire() {

    // adquiero el lock
    omp_set_lock( lock );
} 

  
/**
 * Release the lock
 *
 *  omp_unset_lock
 */
void Lock::Release() {

    // libero el lock
    omp_unset_lock( lock );
}
