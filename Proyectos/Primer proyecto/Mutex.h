#pragma once

#include "Semaforo.h"

class Mutex {
    private:
        Semaforo * mutex;

    public:
        Mutex();
        ~Mutex();
        void Lock();
        void Unlock();
};