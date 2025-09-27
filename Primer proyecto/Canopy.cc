#include "Canopy.h"

Canopy::Canopy( int id ) {
    int monkey;

    this->lock = new Lock();

    this->monkeysCrossed = 0;
    this->monkeysOnRope = 0;

    this->ropeDir = LEFT; // can make it random but harder for consistency to check bugs
    this->previousDir = LEFT;

    this->ropeIndex = id + 1;

    for(monkey = 0; monkey < TROOP; ++monkey) {
        this->state[ monkey ] = QUEUEING;
        this->self[ monkey ] = new Condition();
    }
}

Canopy::~Canopy() {
    int monkey;

    delete this->lock;

    for(monkey = 0; monkey < TROOP; ++monkey) {
        delete this->self[ monkey ];
    }
}

void Canopy::queue( int monkey, Direction dir ) {
    
    // printf( "Monkey %d queued on side %s\n", monkey, ( dir == LEFT ) ? "LEFT" : "RIGHT" );
    // fflush( stdout );

    this->lock->Acquire();
    
    // monkey waiting in queue
    this->state[ monkey ] = QUEUEING;

    // if crossing is not possible, wait for new president elected
    while( !test( monkey, dir ) ) {
        this->self[ monkey ]->Wait( this->lock );
    }

    
    this->monkeysOnRope++; // useful for on-rope / off-rope fork management
    this->monkeysCrossed++; // for rope alternation


    this->lock->Release();
}

void Canopy::cross( int monkey, Direction dir ) {
    int swinging; // time crossing the rope

    this->state[ monkey ] = CROSSING;

    // print the crossing status and direction of the crossing
    printf( "Monkey %d is crossing the rope [%d] %s\n", monkey, ropeIndex, (dir == LEFT) ? "LEFT -> RIGHT" : "RIGHT -> LEFT" );
    fflush( stdout );

    // simulation for crossing time
    swinging = 100000 + (rand() & 0xffff);
    usleep( swinging );
}

void Canopy::leave( int monkey, Direction dir ) {
    int distracted;
    
    this->lock->Acquire();

    this->state[ monkey ] = DONE;

    this->monkeysOnRope--;

    // check for direction change
    if( monkeysOnRope == 0 ) { // no monkeys swinging
        if( monkeysCrossed >= TURNS) {
            
            // rope changes direction
            ropeDir = ( ropeDir == LEFT ) ? RIGHT : LEFT;
            
            // reset my monkey counter
            monkeysCrossed = 0;
        }

        if( ropeDir != previousDir ) {
            // print change of direction
            printf("The rope [%d] changes direction to %s\n", ropeIndex, ( ropeDir == LEFT ) ? "LEFT -> RIGHT" : "RIGHT -> LEFT");
            fflush( stdout );

            previousDir = ropeDir;
        }

        // wake monkeys on hold
        for( distracted = 0; distracted < TROOP; ++distracted ) {
            self[ distracted ]->Signal();
        }
    }

    this->lock->Release();
}

bool Canopy::test( int monkey, Direction dir ) {
    // if rope is empty mimic monkey direction in case there is a single monkey in any side at some point
    if( monkeysOnRope == 0 ) {
        ropeDir = dir;

        if( ropeDir != previousDir ) {
            printf( "The rope [%d] changes direction to %s\n", ropeIndex, ( ropeDir == LEFT ) ? "LEFT -> RIGHT" : "RIGHT -> LEFT" );
            fflush( stdout );

            previousDir = ropeDir;
        }
    }

    // if direction match and TURNS are not yet spent it can cross
    if( ropeDir == dir && monkeysOnRope < TURNS) {
        state[ monkey ] = CROSSING;
        self[ monkey ]->Signal();
        return true;
    }
    return false;
}
