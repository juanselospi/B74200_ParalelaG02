#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#include "Lock.h"
#include "Condition.h"

#define TROOP 10 // number of monkeys
#define TURNS 5 // limit of consecutive monkeys permited from the same direction

enum Direction { LEFT, RIGHT };

class Canopy {
    private:
        Lock * lock;
        Condition * self [ TROOP ];
        enum { QUEUEING, CROSSING, DONE } state[ TROOP ];
        
        Direction ropeDir; // rope current direction
        Direction previousDir; // rope previous direction

        int monkeysOnRope;
        int monkeysCrossed;

        int ropeIndex;

        bool test( int monkey, Direction dir ); // verify if monkey can cross or not

    public:
        Canopy( int id = 0 );
        ~Canopy();
        void queue( int monkey, Direction dir );
        void cross( int monkey, Direction dir );
        void leave( int monkey, Direction dir );

};