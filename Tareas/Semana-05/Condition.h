/**
  *  Representación en C++ de las variables de condición
  *  Para resolver el problema de los filósofos comensales
  *
  *  Author: CI0117 Programación Concurrente
  *
  *  Date:   2020/Set/03
  *
 **/

#include "Lock.h"

class Condition {

   public:
      Condition();
      ~Condition();
      void Wait( Lock * );
      void NotifyOne();
      void NotifyAll();
      void Signal();

   private:
     int workers;
     Lock * internalWaitMechanism;

};

