#include "Canopy.h"

#define TURF 10

struct MonkeyRope
{
    Canopy rope[ 2 ];
};

int Monkey( int who, MonkeyRope * monkeyRope) {
    int stroll;

    srand(getpid());

    // every monkey is spawned at random on left or right
    Direction dir = (rand() % 2 == 0) ? LEFT : RIGHT;

    for ( stroll = 0; stroll < TURF; ++stroll ) {
        // procreastinating time
        int distracted = rand() & 0xffff;
        usleep( distracted );

        int ropeIndex = rand() % 2;
        Canopy * rope = &monkeyRope->rope[ ropeIndex ];

        int ropeNum = ropeIndex + 1;
        printf( "Monkey %d picked rope %d\n", who, ropeNum );
        fflush( stdout );

        // get in queue to jump the rope
        rope->queue( who, dir );

        // crosses the rope
        rope->cross( who, dir );

        // exits the rope
        rope->leave( who, dir );

        // monkeys changes sides on exit
        dir = ( dir == LEFT ) ? RIGHT : LEFT;
    }
    shmdt( monkeyRope ); // unmap shared memo segments to prevent ipcs trash
    exit( 0 );
}

int main( int argc, char ** argv ) {
    long monkeys;
    int monkey, pid, memId;

    MonkeyRope * monkeyRope;
    Canopy * canopy = new Canopy();

    monkeys = 10;
    if ( argc > 1 ) {
        monkeys = atol( argv[ 1 ] );
    }

    // create shared memory
    memId = shmget( IPC_PRIVATE, sizeof( struct MonkeyRope ), IPC_CREAT | 0600 );
    if ( -1 == memId ) {
        perror( "Fail to create shared memory segment" );
        exit( 1 );
    }

    monkeyRope = (struct MonkeyRope *) shmat( memId, NULL, 0 );

    // init both ropes on shared memory
    new (&monkeyRope->rope[ 0 ]) Canopy( 0 );
    new (&monkeyRope->rope[ 1 ]) Canopy( 1 );

    // create monkeys
    for ( monkey = 0; monkey < monkeys; ++monkey ) {
        pid = fork();
        if ( ! pid ) {
            Monkey( monkey, monkeyRope );
        }
    }

    // wait for monkeys to stop
    for ( monkey = 0; monkey < monkeys; ++monkey ) {
        int status;
        pid_t pid = wait( &status );
    }

    // delete stray semaphores
    for( int semId : Semaforo::semIds ) {
        if( -1 == semctl( semId, 0, IPC_RMID ) ) {
            perror( "Semaphore cleanup failed" );
        }
    }

    shmdt( monkeyRope );
    shmctl( memId, IPC_RMID, NULL );

    exit( 0 );
}
