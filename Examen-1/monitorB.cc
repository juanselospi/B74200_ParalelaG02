#include "monitorB.h"

Monitor::Monitor() {
    int process;

    this->lock = new Lock();

    this->procesosEnUso = 0;
    this->procesosEnCola = 0;

    for(process = 0; process < PROCESOS; ++process) {
        this->state[ process ] = QUEUEING;
        this->self[ process ] = new Condition();
    }
}

Monitor::~Monitor() {
    int process;

    delete this->lock;

    for(process = 0; process < PROCESOS; ++process) {
        delete this->self[ process ];
    }
}

void Monitor::queue( int process ) {
    this->lock->Acquire();
    
    // monkey waiting in queue
    this->state[ process ] = QUEUEING;

    this->procesosEnCola++;

    // test if there are already 12 proceses inside
    while( !testIn( process ) ) {
        this->self[ process ]->Wait( this->lock );
    }

    this->procesosEnCola--;
    this->procesosEnUso++; // useful for on-rope / off-rope fork management


    this->lock->Release();
}

void Monitor::enter( int process ) {
    int working; // tiempo usando el recurso compartido

    this->state[ process ] = WORKING;

    // print the working statis
    printf( "Process %d is working...\n", process );
    fflush( stdout );

    // simulation for crossing time
    working = 100000 + (rand() & 0xffff);
    usleep( working );
}

void Monitor::leave( int process ) {
    int distracted;

    this->lock->Acquire();

    // verificar la condicion de salida para evitar el 13 supersticioso
    while( !testOut( process ) ) {
        this->self[ process ]->Wait( this->lock );
    }

    this->state[ process ] = DONE;

    this->procesosEnUso--;
    this->procesosEnCola++;

    // print the working statis
    printf( "Process %d finished...\n", process );
    fflush( stdout );

    // despierta los procesos en espera al terminar
    for( distracted = 0; distracted < PROCESOS; ++distracted ) {
        self[ distracted ]->Signal();
    }

    this->lock->Release();

}

bool Monitor::testIn( int process ) {
    
    // si hay 12 en uso y quiere entrar otro solo puede si hay mas de 1 en cola y entran 2
    if( procesosEnUso == 12 ){
        if( procesosEnCola > 1 ) {
            printf( "Process %d y otro mas entra...\n", process );
            fflush( stdout );
            state[process] = WORKING;
            self[process]->Signal();
            state[process+ 1] = WORKING;
            self[process + 1]->Signal();
            procesosEnUso++;
            procesosEnCola--;

            return true;
        }

    // si hay menos de 12 o mas de 14 procesos trabajando se puede entrar con normalidad
    } else if( procesosEnUso < 12 || procesosEnUso > 14) {
        printf( "Process %d entra...\n", process );
        fflush( stdout );
        state[process] = WORKING;
        self[process]->Signal();
        return true;
    }

    return false;
}

bool Monitor::testOut( int process ) {
    
    // un procesos solo puede abandonar el trabajo si cuando salga el numero de trabajadores es distinto de 13, sino debe esperar
    if(procesosEnUso <= 12 || procesosEnUso > 14) {
        printf( "Process %d termina de trabajar...\n", process );
        fflush( stdout );
        return true;
    }

    return false;
}