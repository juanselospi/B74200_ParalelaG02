/*  Esta clase encapsula las funciones para la utilización de variables de condi
ción
 *
 *  Autor: Programacion Paralela y Concurrente
 *  Fecha: 2020/Set/09
 *
**/

#include <stdexcept>

#include "Condition.h"

/*
 * pthread_cond_init
 */
Condition::Condition() {

   int resultado = -1;

   this->vc = new pthread_cond_t;

   pthread_condattr_t atributos;

   pthread_condattr_init(&atributos);

   resultado = pthread_cond_init(this->vc, &atributos);

   if(resultado != 0) {
      throw std::runtime_error("fallo la inicializacion de Condition");
   }

   pthread_condattr_destroy(&atributos);
}


/*
 * pthread_cond_destroy
 */
Condition::~Condition() {

   pthread_cond_destroy(this->vc);
   delete this->vc;

}


/*
 * pthread_cond_wait
 */
int Condition::Wait( Mutex * mutex ) {
   // return -1;
   int resultado = -1;

   resultado = pthread_cond_wait(this->vc, mutex->getMutex());
   
   if(resultado != 0) {
      throw std::runtime_error("fallo en Wait de Condition");
   }

   return resultado;
}


/*
 * pthread_cond_timedwait
 */
int Condition::TimedWait( Mutex * mutex, const struct timespec * abstime ) {
   // return -1;

   int resultado = -1;

   resultado = pthread_cond_timedwait(this->vc, mutex->getMutex(), abstime);

   if(resultado != 0 && resultado != ETIMEDOUT) {
      throw std::runtime_error("fallo en TimedWait de Condition");
   }

   return resultado;
}


/*
 * pthread_cond_signal
 */
int Condition::Signal() {
   // return -1;
   int resultado = -1;

   resultado = pthread_cond_signal(this->vc);

   if(resultado != 0) {
      throw std::runtime_error("fallo en Singal de Condition");
   }

   return resultado;
}


/*
 * pthread_cond_broadcast
 */
int Condition::Broadcast() {
   // return -1;

   int resultado = -1;

   resultado = pthread_cond_broadcast(this->vc);

   if(resultado != 0) {
      throw std::runtime_error("fallo en Broadcast de Condition");
   }

   return resultado;
}

