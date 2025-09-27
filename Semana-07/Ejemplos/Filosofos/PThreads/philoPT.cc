// Compile with g++ philoPT.cc dinningPh.cc -lpthread
// Unix
//

//#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <mutex>

#include "dinningPh.h"

#define NUMTHRDS 5
int status;

DiningPh dp;
std::mutex mtx;


int main ( int argc, char *argv[] );
void Philo ( int arg );


int main ( int argc, char *argv[] ) {

  int i;
  std::thread philosophers[ NUMTHRDS ];
  for ( i = 0; i < NUMTHRDS; i++ ) {
    philosophers[ i ] = std::thread( Philo, i );
  }

  for ( i = 0; i < NUMTHRDS; i++ ) {
    philosophers[ i ].join();
  }

  std::cout << "\n";
  std::cout << "  Normal end of execution.\n";

  return 0;
}


void Philo( int who ) {

    int eats, thinks;

    for ( int i = 0; i < 10; i++ ) {

        mtx.lock();
        std::cout << "Round " << i << " philo # " << who + 1 << std::endl;
        dp.print();
        std::cout << " Philosopher " << who + 1 << " will try to pickup sticks" << std::endl;
        mtx.unlock();

        dp.pickup( who );
        eats = rand() % 6;
        sleep( eats );

        dp.putdown( who );
        thinks = rand() % 6;
        sleep( thinks );
    }

}

