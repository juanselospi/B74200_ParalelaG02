#include "Condition.h"

Condition::Condition() {

   this->monkeys = 0;
   this->internalWaitMechanism = new Lock();

}

Condition::~Condition() {

   delete this->internalWaitMechanism;

}

void Condition::Wait( Lock * affectedLock ) {

   this->monkeys++;

   this->internalWaitMechanism->Release();
   affectedLock->Release();
   this->internalWaitMechanism->Acquire();
   affectedLock->Acquire();

}

void Condition::NotifyOne() {

   if ( this->monkeys > 0 ) {
      this->monkeys--;

      this->internalWaitMechanism->Release();
   }

}

void Condition::Signal() {

   this->NotifyOne();

}

void Condition::NotifyAll() {

   while ( this->monkeys > 0 ) {
      this->monkeys--;

      this->internalWaitMechanism->Release();
   }

}
