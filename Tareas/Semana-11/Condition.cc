/**
  *  Esta clase encapsula las funciones para la utilizacion de variables de condición
  *
  *  Autor: Programacion Paralela y Concurrente
  *
  *  Fecha: 2025/Oct/31
  *
 **/

#include "Condition.h"

/*
 *  Creates a new condition variable
 *
 *  Uses an internal structure to make workers wait for resources
 *
**/
Condition::Condition() {

   this->workers = 0;

   // To be completed by students

    internalWaitMechanism = new omp_lock_t;
    sem = new omp_lock_t;

    omp_init_lock( internalWaitMechanism );
    omp_init_lock( sem );

    // bloqueo el semaforo inicialmente
    omp_set_lock( sem );
}


/**
 * Destroys our condition variable
**/
Condition::~Condition() {

   // To be completed by students

    omp_destroy_lock( internalWaitMechanism );
    omp_destroy_lock( sem );

    delete internalWaitMechanism;
    delete sem;
}


/**
 *  Wait for the condition controlled by our variable
 *
**/
void Condition::Wait( Lock * affectedLock ) {

   // To be completed by students

    // aumenta contador de hilos que esperan
    omp_set_lock( internalWaitMechanism );
    workers++;
    omp_unset_lock( internalWaitMechanism );

    // libera el lock externo
    affectedLock->Release();

    // bloquea hasta recibir señal
    omp_set_lock( sem );

    // adquiere de nuevo el affectedLock al despertar
    affectedLock->Acquire();
}

  
/**
 *  Notify one worker from the queue, if empty has no effect
 *
**/
void Condition::NotifyOne() {

   // To be completed by students

    omp_set_lock( internalWaitMechanism );

    // si hay workers
    if( workers > 0 ) {

        this->workers--;
        omp_unset_lock( sem );
    }
    
    omp_unset_lock( internalWaitMechanism );
}


/**
 *  Same method as notify one, declared for compatibility with many examples
 *
**/
void Condition::Signal() {

   // To be completed by students
   
    NotifyOne();
}


/**
 *  Signal all workers from the queue, if empty has no effect
 *
**/
void Condition::NotifyAll() {

    omp_set_lock( internalWaitMechanism );

    while ( this->workers > 0 ) {
        
        this->workers--;

        // To be completed by students
        omp_unset_lock( sem );
    }

   omp_unset_lock( internalWaitMechanism );
}
