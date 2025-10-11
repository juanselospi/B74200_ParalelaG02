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

#define PROCESOS 100 // numero de procesos
#define BANEADO 13 // numero de proceso no permitido

class Monitor {
    private:
        Lock * lock;
        Condition * self [ PROCESOS ];
        enum { QUEUEING, WORKING, DONE } state[ PROCESOS ];
        
        int procesosEnUso;
        int procesosEnCola;

        bool testIn( int process ); // verifica si los procesos puede comenzar a trabajar o no

        bool testOut( int process ); // verifica que al dejar de trabajar no queden 13 procesos trabajando

    public:
        Monitor();
        ~Monitor();
        void queue( int process );
        void enter( int process );
        void leave( int process );

};