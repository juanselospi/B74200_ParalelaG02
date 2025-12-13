#pragma once

#include <iostream>

#include "Lock.h"
#include "Condition.h"

#define THINKING 0
#define HUNGRY 1
#define EATING 2

class Mesa {
public:
    int N;
    Lock* lock;
    int* state;
    Condition* self;

    Mesa( int num );
    ~Mesa();

    void pickup( int filosofo );
    void putdown( int filosofo );

private:
    void test( int filosofo );
    
};
