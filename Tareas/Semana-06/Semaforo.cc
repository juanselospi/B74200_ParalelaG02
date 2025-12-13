/**
  *  PThreads
  *  Esta clase encapsula las funciones para la utilizacion de semaforos
  *  Se provee las estructuras para realizar la sincronizacion de trabajadores
  *  a traves de los metodos tradicionales Signal (V), Wait (P)
  *
  *  Autor: Programacion Paralela y Concurrente
  *
  *  Fecha: 2025/Set/16
 **/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdexcept>

#include "Semaforo.h"

/*
 *  sem_init
 */
Semaforo::Semaforo( int inicial ) {

   int resultado = -1;

   this->semId = new sem_t;

   resultado = sem_init(this->semId, 0, inicial);

   if(resultado != 0) {
      throw std::runtime_error("fallo la inicializacion de Semaforo");
   }

}


/*
 *  sem_destroy
 */
Semaforo::~Semaforo() {

   sem_destroy(this->semId);
   delete this->semId;

}


/*
 *  sem_post
 */
int Semaforo::Signal() {
   int resultado = -1;

   resultado = sem_post(this->semId);

   if(resultado != 0) {
      throw std::runtime_error("fallo en Signal de Semaforo");
   }

   return resultado;
}


/*
 *  sem_wait
 */
int Semaforo::Wait() {
   int resultado = -1;

   resultado = sem_wait(this->semId);

   if(resultado != 0) {
      throw std::runtime_error("fallo en Wait de Semaforo");
   }

   return resultado;
}


/*
 *  sem_trywait
 */
int Semaforo::tryWait() {
   int resultado = -1;

   resultado = sem_trywait(this->semId);

   if(resultado != 0 && errno != EAGAIN) {
      throw std::runtime_error("fallo en tryWait de Semaforo");
   }

   return resultado;
}


/*
 *  sem_timedwait
 */
int Semaforo::timedWait( long sec, long nsec ) {
   int resultado = -1;
   struct timespec lapso;

   lapso.tv_nsec = nsec;
   lapso.tv_sec = sec;

   resultado = sem_timedwait(this->semId, &lapso);

   if(resultado != 0 && errno != ETIMEDOUT) {
      throw std::runtime_error("fallo en timedWait de Semaforo");
   }

   return resultado;
}

