/**
  *  Esta clase encapsula las funciones para la utilizacion de variables de condición
  *
  *  Autor: Programacion Paralela y Concurrente
  *
  *  Fecha: 2025/Set/16
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
   this->internalWaitMechanism = new Lock();	// Could be any other mechanism

}


/**
 * Destroys our condition variable
**/
Condition::~Condition() {

   delete this->internalWaitMechanism;

}


/**
 *  Wait for the condition controlled by our variable
 *
**/
void Condition::Wait( Lock * affectedLock ) {

   this->workers++;

   // To be completed by students
   this->internalWaitMechanism->Release();
   affectedLock->Release();
   this->internalWaitMechanism->Acquire();
   affectedLock->Acquire();

} 

  
/**
 *  Notify one worker from the queue, if empty has no effect
 *
**/
void Condition::NotifyOne() {

   if ( this->workers > 0 ) {
      this->workers--;	// One
   // To be completed by students
      this->internalWaitMechanism->Release();
   }

}


/**
 *  Same method as notify one, declared for compatibility with many examples
 *
**/
void Condition::Signal() {

   this->NotifyOne();

}


/**
 *  Signal all workers from the queue, if empty has no effect
 *
**/
void Condition::NotifyAll() {

   while ( this->workers > 0 ) {
      this->workers--;
   // To be completed by students
      this->internalWaitMechanism->Release();
   }

}

