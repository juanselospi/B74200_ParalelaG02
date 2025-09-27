/**
 * Esta clase encapsula las funciones para la utilizacion de Mutexs
 * Se provee los metodos para realizar la sincronizacion de procesos
 * a traves de los llamados tradicionales a Acquire y Release
*/

#ifndef _MUTEX_H
#define _MUTE_HX
#include "Semaphore.h"

class Mutex {
   public:
      Mutex();
      ~Mutex();
      void Lock();
      void Unlock();

   private:
      Semaphore * mutex;

};

#endif
