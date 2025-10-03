#include "Mutex.h"

Mutex::Mutex() {
    this->mutex = new Semaforo( 1, 1 );
}

Mutex::~Mutex() {
    delete this->mutex;
}

void Mutex::Lock() {
    this->mutex->Wait( 0 );
}

void Mutex::Unlock() {
    this->mutex->Signal( 0 );
}