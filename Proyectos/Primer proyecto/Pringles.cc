#include <sys/wait.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <sys/shm.h>

#include "Buzon.h"
#include "Mutex.h"

int Collatz( int trigger ) { // when outcome equals 1 potato explodes
    int outcome;
    if( ( trigger % 2 ) == 0 ) {
        outcome = trigger / 2;
        return outcome;
    } else {
        outcome = 3 * trigger + 1;
        return outcome;
    }

}

void Player( int id, int players, int dir, Buzon * buzon, Mutex * mutex, bool * gameStatus ) {
    Potato explosive;
    bool active = true;

    while( true ) {
        buzon->Recibir( explosive, id + 1 );

        if( explosive.value == -1 ) {
            break;
        } 

        if( !explosive.genuine ) {
            mutex->Lock();

            std::cout << "[INVADER] Faker " << id 
              << " sent fake [" << explosive.value 
              << "] from origin " << explosive.originId 
              << " to [GENUINE] Player " << explosive.destinyId << std::endl;

            mutex->Unlock();

            continue;
        
        } else {
            mutex->Lock();

            std::cout << "[GENUINE] Player " << id 
                    << " got explosive [" << explosive.value 
                    << "] from: " << explosive.originId << std::endl;

            mutex->Unlock();

            if( active == true ) {
                std::cout << " ...generating Collatz!" << std::endl;
                explosive.value = Collatz( explosive.value ); // Collatz for active players

                if( explosive.value == 1 ) { // if exploded become passive
                    mutex->Lock();

                    std::cout << "Potato exploded on player: " << id << std::endl;

                    mutex->Unlock();

                    active = false;
                    explosive.actives--;

                    int next = (dir == 1) ? (id + 1) % players : (id - 1 + players) % players;
                    Potato newPotato;
                    newPotato.mtype = next + 1;
                    newPotato.value = rand() % 5 + 1;
                    newPotato.originId = id;
                    newPotato.destinyId = next;
                    newPotato.genuine = true;
                    newPotato.actives = explosive.actives;
                    buzon->Enviar( newPotato );

                    continue;
                }

                if( explosive.actives == 1 ) {
                    mutex->Lock();
                
                    std::cout << "The WINNER is player: " << id << std::endl;

                    *gameStatus = true;

                    // if( *gameStatus == true ) {
                    //     std::cout << "Game Status: TRUE (from winner)" << std::endl;
                    // }

                    mutex->Unlock();

                    // at this point i already have the winner -> notify only Invader to stop
                    Potato end;
                    end.value = -1;
                    end.mtype = 999;
                    buzon->Enviar( end );

                    for( int winner = 0; winner < players; ++winner ) {
                        if( winner != id) {
                            Potato stop;
                            stop.value = -1;
                            stop.mtype = winner + 1;
                            buzon->Enviar(stop);
                        }
                    }

                    break;
                }
                
                if( explosive.actives > 1 ) {
                    int next = ( dir == 1 ) ? ( id + 1 ) % players : ( id - 1 + players ) % players;

                    sleep( 1 );

                    Potato samePotato;
                    samePotato.mtype = next + 1;
                    samePotato.value = explosive.value; // keep my trigger value if im a passive player
                    samePotato.originId = id;
                    samePotato.destinyId = next;
                    samePotato.genuine = true;
                    samePotato.actives = explosive.actives;
                    buzon->Enviar( samePotato );

                    continue;
                }


            } else {
                // if there is actives left but im not active just send the potatoe without interfering
                if( explosive.actives >= 1 ) {
                    int next = ( dir == 1 ) ? ( id + 1 ) % players : ( id - 1 + players ) % players;

                    sleep( 1 );

                    Potato reusedPotato;
                    reusedPotato.mtype = next + 1;
                    reusedPotato.value = explosive.value; // keep my trigger value if im a passive player
                    reusedPotato.originId = id;
                    reusedPotato.destinyId = next;
                    reusedPotato.genuine = true;
                    reusedPotato.actives = explosive.actives;
                    buzon->Enviar( reusedPotato );

                    continue;
                }
            }
        }
    }

    exit( 0 );
}

void Invader( int players, Buzon * buzon, Mutex * mutex, bool * gameStatus ) { // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAYUDAAAAAAAAAAAAAAAAAAA!!!!!!!!!!!!!
    while (true) {
        
        // std::cout << "Number of Players: " << players << std::endl;

        // if( *gameStatus == false ) {
        //     std::cout << "Game Status: FALSE" << std::endl;
        // } else if( *gameStatus == true ) {
        //     std::cout << "Game Status TRUE" << std::endl;
        // }

        sleep(2);

        if( *gameStatus == true ) {
            break;
        }

        int destiny = rand() % players;
        Potato fake;
        fake.mtype = destiny + 1;
        fake.value = rand() % 100;
        fake.originId = -99;
        fake.destinyId = destiny;
        fake.genuine = false;
        buzon->Enviar( fake );

        mutex->Lock();

        std::cout << "[INVADER] Sent fake message to player " << destiny 
                  << " with value " << fake.value << std::endl;

        mutex->Unlock();
    }

    exit( 0 );
}

int main( int argc, char ** argv ) {
    int players; // how many people are playing
    int kaboom; // potato kaboom code
    int dir; // clockwise, counterclockwise
    
    Buzon * buzon = new Buzon();
    Mutex * mutex = new Mutex();

    // input from terminal
    players = ( argc > 1 ) ? atoi( argv[ 1 ] ) : 5; // if not given by terminal use 5 players
    kaboom = ( argc > 2 ) ? atoi( argv[ 2 ] ) : 19; // if not given kaboom code is 19
    dir = ( argc > 3 ) ? atoi( argv[ 3 ] ) : 1;

    srand( time( NULL ) );

    // shared memory segment
    int shmId = shmget( IPC_PRIVATE, sizeof( bool ), IPC_CREAT | 0666 );
    bool * gameOver = ( bool * ) shmat( shmId, NULL, 0 );
    * gameOver = false;

    // create player
    for( int id = 0; id < players; ++id ) {
        if( fork() == 0 ) {
            Player( id, players, dir, buzon, mutex, gameOver );
        }
    }

    // create invader
    pid_t invaderId;
    if( ( invaderId = fork() ) == 0 ) {
        Invader( players, buzon, mutex, gameOver );
    }

    // set values and send first potatoe
    int start = rand() % players; // random player to send the first message
    Potato potato;
    potato.mtype = start + 1;
    potato.value = kaboom;
    potato.originId = -1;
    potato.destinyId = start;
    potato.genuine = true;
    potato.actives = players;
    buzon->Enviar( potato );

    // wait for game to end;
    while( wait( NULL ) > 0 );

    shmdt( gameOver );
    shmctl( shmId, IPC_RMID, NULL );

    delete buzon;
    delete mutex;

    return 0;

}
