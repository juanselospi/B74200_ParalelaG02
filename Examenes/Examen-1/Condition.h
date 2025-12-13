#pragma once

#include "Lock.h"

class Condition {
    private:
        int suerte;
        Lock * internalWaitMechanism;

    public:
        Condition();
        ~Condition();
        void Wait( Lock * );
        void NotifyOne();
        void NotifyAll();
        void Signal();
};