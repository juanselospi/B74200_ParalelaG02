/**
  *  PThreads
  *
  *  Esta clase encapsula las funciones para la utilizacion de barreras
  *  Una barrera detiene una cantidad definida de hilos, una vez alcanzado
  *  ese limite, todos los hilos continuan a partir del punto de la barrera
  *
  *  Autor: Programacion Paralela y Concurrente
  *
  *  Fecha: 2025/Set/16
 **/

#include <stdexcept>

#include "Barrier.h"


/*
 *  pthread_barrier_init
 */
Barrier::Barrier( int limit ) {

  int resultado = -1;

  this->barrier = new pthread_barrier_t;

  pthread_barrierattr_t atributos;

  pthread_barrierattr_init(&atributos);

  resultado = pthread_barrier_init(this->barrier, &atributos, limit);

  if(resultado != 0) {
      throw std::runtime_error("fallo la inicializacion de Barrier");
   }

   pthread_barrierattr_destroy(&atributos);
}


/*
 * pthread_barrier_destroy
 */
Barrier::~Barrier() {

  pthread_barrier_destroy(this->barrier);
  delete this->barrier;

}


/*
 * pthread_barrier_wait
 */
void Barrier::Wait() {

  int resultado = -1;

   resultado = pthread_barrier_wait(this->barrier);

   if(resultado != 0 && resultado != PTHREAD_BARRIER_SERIAL_THREAD) {
      throw std::runtime_error("fallo en Wait de Barrier");
   }

}

