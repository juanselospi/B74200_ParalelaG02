#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

#include "Mesa.h"
#include "Lock.h"
#include "Condition.h"

using namespace std;

void Filosofo( int id, Mesa* mesa ) {

    for( int i = 0; i < 3; ++i ) {

        #pragma omp critical
        cout << "[Filosofo " << id << "] pensando..." << endl;

        usleep( 100000 + rand() % 100000 );

        mesa->pickup( id );

        #pragma omp critical
        cout << "[Filosofo " << id << "] comiendo..." << endl;

        usleep( 100000 + rand() % 100000 );

        mesa->putdown( id );

        #pragma omp critical
        cout << "[Filosofo " << id << "] terminó de comer" << endl;

    }
}

int main() {

    srand( time( NULL ) );

    int N = 5;
    Mesa* mesa = new Mesa( N );

    #pragma omp parallel num_threads( N )
    {
        int id = omp_get_thread_num();
        Filosofo( id, mesa );
    }

    delete mesa;
    return 0;
}
