#include "monitorB.h"

#define CICLOS 30

struct PreguntaB
{
    Monitor monitores;
};

int Proceso( int who, PreguntaB * monitor) {
    int shift; // trabajo

    for ( shift = 0; shift < CICLOS; ++shift) {
        // procreastinating time
        int distracted = rand() & 0xffff;
        usleep( distracted );

        Monitor * monitores = &monitor->monitores;

        // get in queue to work
        monitores->queue( who );

        // start working
        monitores->enter( who );

        // exits work
        monitores->leave( who );

    }

    shmdt( monitor ); // unmap shared memo segments to prevent ipcs trash
    exit( 0 );
}

int main( int argc, char ** argv ) {
    long procesos;
    int proceso, pid, memId;

    PreguntaB * preguntaB;
    Monitor * supervisor = new Monitor();

    procesos = 100;
    if ( argc > 1 ) {
        procesos = atol( argv[ 1 ] );
    }

    // create shared memory
    memId = shmget( IPC_PRIVATE, sizeof( struct PreguntaB ), IPC_CREAT | 0600 );
    if ( -1 == memId ) {
        perror( "Fail to create shared memory segment" );
        exit( 1 );
    }

    preguntaB = ( struct PreguntaB * ) shmat( memId, NULL, 0 );

    // init struct on shared memory
    new (&preguntaB->monitores) Monitor();

    // create proceses
    for ( proceso = 0; proceso < procesos; ++proceso ) {
        pid = fork();
        if ( ! pid ) {
            Proceso( proceso, preguntaB );
        }
    }

    // wait for process to stop
    for ( proceso = 0; proceso < procesos; ++proceso ) {
        int status;
        pid_t pid = wait( &status );
    }

    // delete stray semaphores
    for( int semId : Semaforo::semIds ) {
        if( -1 == semctl( semId, 0, IPC_RMID ) ) {
            perror( "Semaphore cleanup failed" );
        }
    }

    shmdt( preguntaB );
    shmctl( memId, IPC_RMID, NULL );

    exit( 0 );
}