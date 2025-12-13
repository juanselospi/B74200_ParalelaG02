#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <unistd.h>

#include "Lock.h"
#include "Semaphore.h"

using namespace std;

struct Papa {

    int valor;
    bool genuina;
    int origen;
    int destino;
};


// Función Collatz
int Collatz( int valor ) {

    if ( valor % 2 == 0 )
        return valor / 2;
    else
        return 3 * valor + 1;
}

int main( int argc, char* argv[] ) {

    srand( time( NULL ) );

    int numJugadores = 5;
    int valorInicial = 20;
    int dir = 1;

    if ( argc >= 3 ) {

        numJugadores = atoi( argv[ 1 ] );
        valorInicial = atoi( argv[ 2 ] );
    }

    if ( argc >= 4 ) {

        dir = atoi( argv[ 3 ] );

        if( dir != 1 && dir != -1 ){
             dir = 1;
        }
    }

    cout << "[ OpenMP ] Iniciando juego de la papa caliente\n";

    Papa papa;
    papa.valor = valorInicial;
    papa.genuina = true;
    papa.destino = 0;
    papa.origen = -1;

    bool explotada = false;
    int turno = 0;
    bool hayFake = false;
    Papa fakePapa;

    Lock lock;
    Semaphore sem( 1 );

    // jugadores activos y contador de jugadores vivos
    bool activo[ numJugadores ];
    int jugadoresVivos = numJugadores;
    for( int i=0; i<numJugadores; i++ ){
        activo[ i ] = true;
    }

    #pragma omp parallel num_threads( numJugadores + 1 ) shared( papa, explotada, turno, sem, lock, hayFake, fakePapa, activo, jugadoresVivos, dir )
    {
        int id = omp_get_thread_num();

        if( id == numJugadores ) {

            while( !explotada ) {
                #pragma omp flush( explotada )

                if( explotada ) {
                    break;
                }

                int objetivo = rand() % numJugadores;
                fakePapa.valor = rand() % 100 + 50;
                fakePapa.genuina = false;
                fakePapa.destino = objetivo;
                fakePapa.origen = -99;

                lock.Acquire();
                hayFake = true;

                cout << "[INVADER] Envia papa falsa a jugador " << objetivo << " con valor " << fakePapa.valor << endl;
                lock.Release();

                // espera aleatoria entre 2 y 3 segundos
                usleep((rand() % 1000 + 2000) * 1000);
            }

        } else {

            while( !explotada ) {

                sem.Wait();
                lock.Acquire();

                if( hayFake && fakePapa.destino == id ) {
                    
                    cout << "[Jugador " << id << "] ignora la papa falsa (" << fakePapa.valor << ")\n";
                    hayFake = false;
                }

                if( id == turno && activo[ id ] && !explotada ) {

                    cout << "[Jugador " << id << "] recibe papa (" << papa.valor << ")\n";

                    // aplicar Collatz
                    papa.valor = Collatz( papa.valor );

                    if( papa.valor == 1 ) {

                        cout << "[Jugador " << id << "] explotó la papa!\n";
                        activo[ id ] = false;
                        jugadoresVivos--;

                        // revisar ganador
                        if( jugadoresVivos == 1 ) {

                            for( int j=0; j<numJugadores; j++ ) {
                                
                                if( activo[ j ] ) {
                                    
                                    cout << "[Jugador " << j << "] es el GANADOR!\n";
                                    break;
                                }

                            }

                            explotada = true;

                        } else {

                            int next;

                            while ( true ) {
                                if ( dir == 1 ) {

                                    next = ( turno + 1 ) % numJugadores;
                                } else {

                                    next = ( turno - 1 + numJugadores ) % numJugadores;
                                }

                                turno = next;

                                if ( activo[ turno ] ) {

                                    break;
                                }
                            }

                            // nueva papa aleatoria
                            papa.valor = rand() % 6 + 5;
                        }

                    } else {

                        int next;

                        while( true ) {

                            if( dir == 1 ) {

                                next = turno + 1;

                                if( next >= numJugadores ) {

                                    next = 0;
                                }
                                
                            } else {

                                next = turno - 1;

                                if( next < 0 ) {

                                    next = numJugadores - 1;
                                }
                            }

                            turno = next;

                            if( activo [ turno ] ) {

                                break;
                            }
                        }

                        cout << "[Jugador " << id << "] pasa la papa a " << turno << endl;
                    }

                    // espera 1 segundo
                    sleep( 1 );
                }

                lock.Release();
                sem.Signal();
            }
        }
    }

    cout << "\nFIN del juego.\n";
    return 0;
}
