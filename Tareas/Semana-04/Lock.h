/**
  *  Author: Programacion Concurrente (Francisco Arroyo)
  *
  *  Version: 2025/Ago/29
  * Esta clase encapsula las funciones para la utilizacion de Locks
  * Se provee los metodos para realizar la sincronizacion de procesos
  * a traves de los llamados tradicionales a Acquire y Release
 **/

#ifndef _LOCK_H
#define _LOCK_H
#include "Semaforo.h"

class Lock {
   public:
      Lock();
      ~Lock();
      void Acquire();
      void Release();

   private:
      Semaforo * lock;

};

#endif

