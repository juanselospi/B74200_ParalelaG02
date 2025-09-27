/**
  * PThreads
  *
  *  Esta clase encapsula las funciones para la utilizacion de mutex
  *  Se provee las estructuras para realizar la sincronizacion de trabajadores
  *  a traves de los metodos tradicionales Lock, Unlock
  *
  *  Autor: Programacion Paralela y Concurrente
  *
  *  Fecha: 2025/Set/16
 **/

#ifndef _MUTEX_H
#define _MUTEX_H

#include <pthread.h>

class Mutex {
public:
   Mutex();
   ~Mutex();
   int Lock();
   int TryLock();
   int Unlock();
   pthread_mutex_t * getMutex();

private:
   pthread_mutex_t * mutex;

};

#endif

