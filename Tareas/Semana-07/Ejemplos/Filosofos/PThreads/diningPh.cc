#include <stdio.h>
#include "dinningPh.h"

DiningPh::DiningPh() {

    for ( int i = 0; i < 5; i++ ) {
        state[ i ] = Thinking;
    }

}


DiningPh::~DiningPh() {
}


void DiningPh::pickup( long who ) {

    std::unique_lock<std::mutex> lock( dp );

    state[ who ] = Hungry;
    test( who );
    if ( state[ who ] == Hungry )
        self[ who ].wait( lock );

}


void DiningPh::putdown( long who ) {

    std::unique_lock<std::mutex> lock( dp );

    state[ who ] = Thinking;
    test( (who + 4) % 5 );
    test( (who + 1) % 5 );

}


void DiningPh::test( long i ) {

    if ( ( state[ (i + 4) % 5 ] != Eating ) && 
         ( state[ i ] == Hungry ) && 
         ( state[ (i + 1) % 5] != Eating ) ) {
        state[ i ] = Eating;
        self[ i ].notify_one();

    }

}

void DiningPh::print() {

    for ( int i = 0; i < 5; i++ ) {
        printf( "Philosopher %d is %s \n", i + 1, (state[i]==Hungry)?"Hungry":(state[i]==Thinking)?"Thinking":"Eating");

    }

}

