#pragma once

#include <stdio.h>
#include <unistd.h>

class Semaforo {
public:
    Semaforo( int = 0);
    ~Semaforo();
    
    int Signal();
    int Wait();
private:
    int id;
};
