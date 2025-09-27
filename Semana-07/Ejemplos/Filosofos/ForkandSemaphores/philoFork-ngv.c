/**
 *  Dining Philosophers solution using simultaneous Wait and Signal on a Unix semaphore array
 *  This solution uses a shared data segment passed by parameter to methods, no global variables
 *  Shared memory segment is created using shm* system calls
 *
 *  @author	CI-0117 Programación Concurrente (Francisco Arroyo)
 *  @date	2019/Oct
 */


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <curses.h>

#include <sys/ipc.h>
#include <sys/sem.h>	// semget, semctl, semop
#include <sys/shm.h>	// shmget, shmat, shmdt, shmctl: process shared memory management

#define NUMWORKERS 5

/**
 *  Shared data record type definition
 */
typedef struct sharedData {
   int table;		// One semaphore for each chopstick
   int mutex;		// Semaphore that eed to be initialized to 1, control access to display
} sharedDataT;


// Function prototype declaration
int main ( int, char *argv[] );
void Philo ( int, sharedDataT * );
void displayPhilosopher( int, int, sharedDataT * );

/*
 *  Curses display
 *  Display the title, no race condition, only main process use it
 *
 */
void displayTitle() {
   WINDOW * title;
   char title0[ 128 ] = "\t CI-0117 Programación Concurrente\n";
   char title1[ 128 ] = "\t ------------------------------------------------------------\n";
   char title2[ 128 ] = "\t| Process | Philosopher | Status | Chopstick 1 | Chopstick 2 |\n";
   char title3[ 128 ] = "\t|------------------------------------------------------------|";

   title = newwin( 4, 80, 0, 1 );
   waddstr( title, title0 );
   waddstr( title, title1 );
   waddstr( title, title2 );
   waddstr( title, title3 );
   wrefresh( title );

}


/**
 *  Display philosopher information using curses
 *  Create a "window" for each philosopher at different positions
 *  One display, many processes, we need to control race conditions: mutex
 */
void displayPhilosopher( int who, int state, sharedDataT * data ) {
   int status;
   struct sembuf display;
   WINDOW * philoWin;
   int posx, posy;
   char buffer[ 128 ];
   char bottomLine[ 128 ] = "\t|------------------------------------------------------------|\n";
   char lastLine[ 128 ] = "\t ------------------------------------------------------------";
   char chop1[ 8 ] = "--";
   char chop2[ 8 ] = "--";
   char displayState[ 8 ] = "";

   switch ( state ) {
      case 0:	// Thinking
         sprintf( displayState, "%6s", "Think " );
         break;
      case 1:	// Hungry
         sprintf( displayState, "%6s", "Hungry" );
         break;
      case 2:	// Eating
         sprintf( displayState, "%6s", "Eating" );
         sprintf( chop1, "%2d", who );
         sprintf( chop2, "%2d", (who + 1) % NUMWORKERS );
         break;
      case 3:	// Finished
         sprintf( displayState, "%6s", "Ended " );
         break;
   }

// Choose window position according to philospher number, depends on title
   posy = (who + 2) * 2;
   posx = 1;

// Semaphore wait operation
   display.sem_num = 0;
   display.sem_op  = -1;
   display.sem_flg = 0;
   status = semop( data->mutex, & display, 1 );
   if ( status ) {
      perror( "displayPhilo, wait" );
      exit( 20 );
   }

// Create curses window to display specific data from each philosopher
   philoWin = newwin( 2, 80, posy, posx );	// nlines, ncols, beginy, beginx

   sprintf( buffer, "\t|  %5d  |      %1d      | %6s |     %2s      |     %2s      |\n", getpid(), who + 1, displayState, chop1, chop2 );
   waddstr( philoWin, buffer );
// Check last line to display
   if ( who + 1 == NUMWORKERS ) {
      waddstr( philoWin, lastLine );
   } else {
      waddstr( philoWin, bottomLine );
   }

   wrefresh( philoWin );

   delwin( philoWin );

// Semaphore signal operation
   display.sem_op = 1;
   status = semop( data->mutex, & display, 1 );
   if ( status ) {
      perror( "displayPhilo, signal" );
      exit( 21 );
   }

}


/**
 * Semaphore initialization in shared segment
 */
void init( sharedDataT * data ) {
// Semaphore structure declaration
   union semun {
      int              val;    /* Value for SETVAL */
      struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
      unsigned short  *array;  /* Array for GETALL, SETALL */
      struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
   } initialVal;

   int status, chopstick;

   data->table = semget( 0x00ECC1, NUMWORKERS, 0600 | IPC_CREAT );
   if ( -1 == data->table ) {
      perror( "main table creation" );
      exit( 10 );
   }
   initialVal.val = 1;
   for ( chopstick = 0; chopstick < NUMWORKERS; chopstick++ ) {
      status = semctl( data->table, chopstick, SETVAL, initialVal );
      if ( -1 == status ) {
         perror( "main, table initial values" );
         exit( 11 );
      }
   }
// Mutex semaphore initialization
   data->mutex = semget( 0x00ECC2, 1, 0600 | IPC_CREAT );
   if ( -1 == data->mutex ) {
      perror( "main, mutex creation" );
      exit( 12 );
   }
   initialVal.val = 1;
   status = semctl( data->mutex, 0, SETVAL, initialVal );
   if ( -1 == status ) {
      perror( "main, mutex initial value" );
      exit( 13 );
   }

}


/**
 *
 */
int main ( int argc, char *argv[] ) {

   int i, pindex, worker, status, shmid;
   pid_t workers[ NUMWORKERS ];
   sharedDataT * shData;

// curses initialization
   initscr();
   cbreak();
   noecho();
   displayTitle();

   srand( time( NULL ) );	// Create a new random seed

// Create and initialice memory shared segment
   shmid = shmget( 0x00ECC1, sizeof( sharedDataT ), 0600 | IPC_CREAT );
   if ( -1 == shmid ) {
      perror( "main, shared memory creation" );
      exit( 14 );
   }
   shData = ( sharedDataT * ) shmat( shmid, NULL, 0 );
   init( shData );

   for ( i = 0; i < NUMWORKERS; i++ ) {	// Start workers
      status = fork();
      if ( ! status ) {	// Child code
         Philo( i, shData );
      } else {
         workers[ i ] = status;
      }
   }

   for ( i = 0; i < NUMWORKERS; i++ ) {
      worker = waitpid( -1, &status, 0 );	// Wait for any of the started processes
//     worker = waitpid( workers[ i ], &status, 0 );	// Wait for started processed in order

      pindex = -1;
      do {	// Find out which worker finished
         pindex++;
      } while ( pindex < NUMWORKERS && worker != workers[ pindex ] );

      displayPhilosopher( pindex, 3, shData );
      if ( -1 == worker ) {
         perror( "main, fork" );
         exit( 5 );
      }
   }

   status = semctl( shData->table, 0, IPC_RMID );
   if ( -1 == status ) {
      perror( "main, table destroy" );
      exit( 15 );
   }
   status = semctl( shData->mutex, 0, IPC_RMID ); 
   if ( -1 == status ) {
      perror( "main, mutex destroy" );
      exit( 16 );
   }

   status = (int) shmdt( shData );
   if ( -1 == status ) {
      perror( "main, shared memory detach" );
      exit( 17 );
   }
   status = shmctl( shmid, IPC_RMID, NULL );
   if ( -1 == status ) {
      perror( "main, shared memory delete" );
      exit( 18 );
   }
   echo();
   nocbreak();
   endwin();

   printf( "\nNormal end of execution.\n" );

   return 0;

}


/**
 *  Do a simultaneous signal operation on two semaphores
 *  @param	int	first semaphore, representing philospher left chopstick
 *  @param	int	second semaphore, representing philosopher right chopstick
 */
void semSignal( int firstSem, int secondSem, sharedDataT * data ) {
   int status;
   struct sembuf signal[ 2 ];

// Fist semaphore
   signal[ 0 ].sem_num = firstSem;
   signal[ 0 ].sem_op  = 1;
   signal[ 0 ].sem_flg = 0;
// Seconde sempahore
   signal[ 1 ].sem_num = secondSem;
   signal[ 1 ].sem_op  = 1;
   signal[ 1 ].sem_flg = 0;

   status = semop( data->table, signal, 2 );
   if ( -1 == status ) {
      perror( "Semaphore signal" );
      exit( 18 );
   }

}


/**
 *  Do a simultaneous wait operation on two semaphores
 *  @param	int	first semaphore, representing philospher left chopstick
 *  @param	int	second semaphore, representing philosopher right chopstick
 */
void semWait( int firstSem, int secondSem, sharedDataT * data ) {
   int status;
   struct sembuf wait[ 2 ];

// Fist semaphore
   wait[ 0 ].sem_num = firstSem;
   wait[ 0 ].sem_op  = -1;
   wait[ 0 ].sem_flg = 0;
// Seconde sempahore
   wait[ 1 ].sem_num = secondSem;
   wait[ 1 ].sem_op  = -1;
   wait[ 1 ].sem_flg = 0;

   status = semop( data->table, wait, 2 );
   if ( -1 == status ) {
      perror( "Semaphore wait" );
      exit( 19 );
   }

}


/**
 *  Simulates the philosopher behaviour repeat for ever (sleep, eat)
 *  Each process do the cycle 10 times
 */
void Philo( int who, sharedDataT * data ) {

    int eats, thinks;

    for ( int i = 0; i < 10; i++ ) {	// Each worker takes 10 rounds

// Initial think cycle (sleep)
        displayPhilosopher( who, 0, data );
        thinks = rand() % 2000000;
        usleep( thinks );

        displayPhilosopher( who, 1, data );
        semWait( who, (who + 1) % NUMWORKERS, data );	// Do a simultaneous wait on both semaphores
// Eat time
        eats = rand() % 2000000;
        displayPhilosopher( who, 2, data );
        usleep( eats );
// Return chopsticks
        semSignal( who, (who + 1) % NUMWORKERS, data );	// Do a simultaneous signal of both semaphores

// Second think cycle
        thinks = rand() % 2000000;
        displayPhilosopher( who, 0, data );
        usleep( thinks );

    }

    _exit( 0 );

}

