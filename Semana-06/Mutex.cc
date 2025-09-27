/**
  * PThreads
  *
  *  Implantacion de la clase Mutex
  *
  *  Autor: Programacion Paralela y Concurrente (Francisco Arroyo)
  *
  *  Fecha: 2025/Set/16
 **/

#include <stdlib.h>
#include <pthread.h>
#include <stdexcept>

#include "Mutex.h"


/**
 *
 *  pthread_mutex_init()
 *
**/
Mutex::Mutex() {
   // int resultado = 0;
   // pthread_mutexattr_t * atributos;

   int resultado = -1;

   this->mutex = new pthread_mutex_t; // reservo memo para el mutex

   pthread_mutexattr_t atributos; // arriba era un puntero sin memoria asignada, mejor pasar por referencia &

   pthread_mutexattr_init(&atributos); // inicializo los atributos del array


   resultado = pthread_mutex_init(this->mutex, &atributos); // inicializo el mutex con los atributos

   if(resultado != 0) {
      throw std::runtime_error("fallo la inicializacion del Mutex");
   }

   pthread_mutexattr_destroy(&atributos); // ya los use para iniciar el mutex y no los necesito

}


/**
 *
 * pthread_mutex_destroy()
 *
**/
Mutex::~Mutex() {

   pthread_mutex_destroy(this->mutex);
   delete this->mutex;

}


/**
 *
 * pthread_mutex_lock()
 *
**/
int Mutex::Lock() {
   int resultado = -1;

   resultado = pthread_mutex_lock(this->mutex);

   if(resultado != 0) {
      throw std::runtime_error("fallo en Lock del Mutex");
   }

   return resultado;

}


/**
 *  pthread_mutex_trylock()
**/
int Mutex::TryLock() {
   int resultado = -1;

   resultado = pthread_mutex_trylock(this->mutex);

   if(resultado != 0 && resultado != EBUSY) {
      throw std::runtime_error("fallo en TryLock del Mutex");
   }

   return resultado;

}

/**
 *  pthread_mutex_unlock()
**/
int Mutex::Unlock() {
   int resultado = -1;

   resultado = pthread_mutex_unlock(this->mutex);

   if(resultado != 0) {
      throw std::runtime_error("fallo en Unlock del Mutex");
   }
   
   return resultado;

}


/**
 * 
**/
pthread_mutex_t * Mutex::getMutex() {

   return this->mutex;

}

