/**
  *  Representación en C++ de las variables de condición
  *  Para resolver el problema de los filósofos comensales
  *
  *  Author: CI0117 Programación Concurrente
  *
  *  Date:   2025/Oct/31
  *
 **/
#pragma once

#include <omp.h>

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
     omp_lock_t * internalWaitMechanism;
     omp_lock_t * sem;

};
