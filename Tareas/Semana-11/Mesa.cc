#include "Mesa.h"

Mesa::Mesa( int num ) {

    N = num;
    lock = new Lock();
    state = new int[ N ];
    self = new Condition[ N ];

    for( int i = 0; i < N; i++ ) {

        state[ i ] = THINKING;
    }
}

Mesa::~Mesa() {

    delete lock;
    delete[] state;
    delete[] self;
}

void Mesa::pickup( int filosofo ) {

    lock->Acquire();

    state[ filosofo ] = HUNGRY;
    test( filosofo );

    if( state[ filosofo ] != EATING ) {

        self[ filosofo ].Wait( lock );
    }

    lock->Release();
}

void Mesa::putdown( int filosofo ) {

    lock->Acquire();

    state[ filosofo ] = THINKING;

    test( ( filosofo + N - 1 ) % N );
    test( ( filosofo + 1 ) % N );

    lock->Release();
}

void Mesa::test( int filosofo ) {

    int left = ( filosofo + N - 1 ) % N;
    int right = ( filosofo + 1 ) % N;

    if( state[ left ] != EATING && state[ right ] != EATING && state[ filosofo ] == HUNGRY ) {

        state[ filosofo ] = EATING;
        self[ filosofo ].Signal();
    }
}
