#pragma once

#include "Semaforo.h"

class Lock {
    private:
        Semaforo * lock;

    public:
        Lock();
        ~Lock();
        void Acquire();
        void Release();
};
