#include <thread>
#include <atomic>
#include <iostream>

std::atomic<int> ca (0);
int cr = 0;

int run( int x ) {
    for(int n = 0; n < 1000; ++n) {
        cr++;
        ca++;
    }
    return 0;
}

int main(void) {
    int n;
    std::thread hilos[10];

    for ( n = 0; n < 10; ++n ) {
        hilos[ n ]= std::thread( run, n );
    }

    for ( n = 0; n < 10; ++n ) {
        hilos[ n ].join();
    }

    printf( "The atomic counter is %d\n",  (int) ca);	// Hay que agregarle el "cast", de otra manera saca un error de compilacion
    std::cout << "The non-atomic counter is " << cr << std::endl;	// En este caso el operador "<<" esta sobrecargado
    std::cout << "The atomic counter is " << ca << std::endl;

}
