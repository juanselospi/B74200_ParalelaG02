#include "Condition.h"

Condition::Condition() {

   this->suerte = 0;
   this->internalWaitMechanism = new Lock();

}

Condition::~Condition() {

   delete this->internalWaitMechanism;

}

void Condition::Wait( Lock * affectedLock ) {

   this->suerte++;

   this->internalWaitMechanism->Release();
   affectedLock->Release();
   this->internalWaitMechanism->Acquire();
   affectedLock->Acquire();

}

void Condition::NotifyOne() {

   if ( this->suerte > 0 ) {
      this->suerte--;

      this->internalWaitMechanism->Release();
   }

}

void Condition::Signal() {

   this->NotifyOne();

}

void Condition::NotifyAll() {

   while ( this->suerte > 0 ) {
      this->suerte--;

      this->internalWaitMechanism->Release();
   }

}
