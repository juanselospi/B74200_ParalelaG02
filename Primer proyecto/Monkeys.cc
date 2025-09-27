#include "Canopy.h"

#define TURF 10

struct MonkeyRope
{
    Canopy rope;
};

int Monkey( int who, Canopy * rope) {
    int stroll;

    srand(getpid());

    // every monkey is spawned at random on left or right
    Direction dir = (rand() % 2 == 0) ? LEFT : RIGHT;

    for ( stroll = 0; stroll < TURF; ++stroll ) {
        // procreastinating time
        int distracted = rand() & 0xffff;
        usleep( distracted );

        // get in queue to jump the rope
        rope->queue( who, dir );

        // crosses the rope
        rope->cross( who, dir );

        // exits the rope
        rope->leave( who, dir );

        // monkeys changes sides on exit
        dir = ( dir == LEFT ) ? RIGHT : LEFT;
    }

    exit( 0 );
}

int main( int argc, char ** argv ) {
    long monkeys;
    int monkey, pid, memId;

    MonkeyRope * monkeyRope;
    Canopy * canopy = new Canopy();

    monkeys = 5;
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
    memcpy( monkeyRope, canopy, sizeof( Canopy ) );	// Copy object to shared segment

    // create monkeys
    for ( monkey = 0; monkey < monkeys; ++monkey ) {
        pid = fork();
        if ( ! pid ) {
            Monkey( monkey, & monkeyRope->rope );

            printf("valor de la cuerda: %d", monkeyRope); // para verificar que todos los monos acceden al mismo segmento de cuerda
        }
    }

    // wait for monkeys to stop
    for ( monkey = 0; monkey < monkeys; ++monkey ) {
        int status;
        pid_t pid = wait( &status );
    }

   shmdt( monkeyRope );
   shmctl( memId, IPC_RMID, NULL );
}