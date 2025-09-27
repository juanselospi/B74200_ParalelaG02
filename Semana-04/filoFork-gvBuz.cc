#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <curses.h>

#include "Buzon.h"
#include "Mutex.h"

#define NUMWORKERS 5

Buzon * buzon;
Mutex * mutex;

int main ( int argc, char *argv[] );
void Philo ( int arg );

/*
 *  Curses display
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
 */
void displayPhilosopher( int who, int state ) {
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

   mutex->Lock();
   philoWin = newwin( 2, 80, posy, posx );	// nlines, ncols, beginy, beginx

   if ( 4 >= who ) {
      sprintf( buffer, "\t|  %5d  |      %1d      | %6s |     %2s      |     %2s      |\n", getpid(), who + 1, displayState, chop1, chop2 );
   } else {
      sprintf( buffer, "\n" );
   }
   waddstr( philoWin, buffer );
// Check last line to display
   if ( who + 1 == NUMWORKERS ) {
      waddstr( philoWin, lastLine );
   } else {
      waddstr( philoWin, bottomLine );
   }

   wrefresh( philoWin );

   delwin( philoWin );

   mutex->Unlock();

}

int main ( int argc, char *argv[] ) {

   int i, pindex, worker, status;
   pid_t workers[ NUMWORKERS ];

// curses initialization
   initscr();
   cbreak();
   noecho();
   displayTitle();

   srand( time( NULL ) );
   buzon = new Buzon();
   mutex = new Mutex();

   for (i = 0; i < NUMWORKERS; i++) {
      buzon->Enviar("Tenedor", i, i + 1);
   }

   for ( i = 0; i < NUMWORKERS; i++ ) {	// Start workers
      status = fork();
      if ( ! status ) {	// Child code
         Philo( i );
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

      displayPhilosopher( pindex, 3 );
      if ( -1 == worker ) {
         perror( "main" );
      }
   }

   delete mutex;
   delete buzon;

   echo();
   nocbreak();
   endwin();

   printf( "\nNormal end of execution.\n" );

   return 0;

}

/**
 *  Simulates the philosopher behaviour repeat for ever (sleep, eat)
 *  Each process do the cycle 10 times
 */
void Philo( int who ) {

    int eats, thinks;

    Mensaje m1, m2;

    for ( int i = 0; i < 10; i++ ) {	// Each worker takes 10 rounds

// Initial think cycle (sleep)
        displayPhilosopher( who, 0 );
        thinks = rand() % 2000000;
        usleep( thinks );		// Initial sleep

        displayPhilosopher( who, 1 );

        // Recibir forks
        if (who == NUMWORKERS - 1) {
            buzon->Recibir(&m2, sizeof(Mensaje) - sizeof(long), (who + 1) % NUMWORKERS + 1);
            buzon->Recibir(&m1, sizeof(Mensaje) - sizeof(long), who + 1);
        } else {
            buzon->Recibir(&m1, sizeof(Mensaje) - sizeof(long), who + 1);
            buzon->Recibir(&m2, sizeof(Mensaje) - sizeof(long), (who + 1) % NUMWORKERS + 1);
        }



        eats = rand() % 2000000;
        displayPhilosopher( who, 2 );
        usleep( eats );

        // Devolver forks
        buzon->Enviar("Tenedor", m1.times, who + 1);
        buzon->Enviar("Tenedor", m2.times, (who + 1) % NUMWORKERS + 1);

// Second think cycle
        thinks = rand() % 2000000;
        displayPhilosopher( who, 0 );
        usleep( thinks );

    }

    _exit( 0 );

}
