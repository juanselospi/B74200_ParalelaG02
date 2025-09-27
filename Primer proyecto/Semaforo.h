#pragma once

#include <stdexcept>
#include <sys/types.h>
#include <sys/sem.h>

class Semaforo {
    private:
        int id;		// Identificador del semaforo
        pid_t owner;	// Resource owner
        int nsems;	// Cantidad de semaforos en el arreglo

    public:
        Semaforo( int, int = 0 );		// semaphore quantity, initial value
        ~Semaforo();			// remove semaphore array
        int Signal( int = 0 );		// First array element operation
        int Wait( int = 0 );		// First array element operation
        void SP( int, int );		// Wait for all semaphore in array
        void SV( int, int );		// Signal all semaphores in array
};