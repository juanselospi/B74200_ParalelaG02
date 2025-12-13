/**
  *  C++ class to encapsulate Unix semaphore intrinsic structures and system calls
  *
  *  Author: Programacion Concurrente (Francisco Arroyo)
  *
  *  Version: 2025/Oct/21
  *
 **/

#include <omp.h>

#ifndef SEMAPHORE
#define SEMAPHORE

class Semaphore {
   public:
      Semaphore( int = 0 );	// initial value
      ~Semaphore();		// remove semaphore 
      int Signal();		// element operation
      int Wait();		// element operation

   private:
      omp_lock_t * mutex;	// Value variable access
      omp_lock_t * sem;		// Resource blocker
      int owner;		// Resource owner
      int value;		// Semaphore value
};
#endif
