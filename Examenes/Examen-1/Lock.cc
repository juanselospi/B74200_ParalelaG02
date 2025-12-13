#include "Lock.h"

Lock::Lock() {

   this->lock = new Semaforo( 1, 1 );

}

Lock::~Lock() {

   delete this->lock;

}

void Lock::Acquire() {

   this->lock->Wait(0);

}

void Lock::Release() {

   this->lock->Signal(0);

}
