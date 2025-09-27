/**
  *  Author: Programacion Concurrente (Francisco Arroyo)
  *
  *  Version: 2025/Ago/29
  *
  *  Mother Nature solution using simultaneous Wait and Signal on a Unix semaphore array
  *  This solution uses a global array of semaphores "table"
  *      main method, create, initializes, and destroy "table" array
  *      Philo method, uses the "table" array to access concurrently atoms and form water molecules
 **/


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <curses.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include "Semaforo.h"
#include "Mutex.h"

#define NUMH 6
#define NUMO 3
#define TOTAL 9

Semaforo * safeCount;
Semaforo * bubble; // Agrupar H2O
Mutex * mutex;

// Mis contadores para atomos de Hidrogeno y Oxigeno listos
int *atomsH = nullptr;
int *atomsO = nullptr;

void workerH(int who);
void workerO(int who);

void moleculePrinter(int who, char atom, const char * state);

/*
 *  Curses display
 */
void displayTitle() {
   WINDOW * title;
   char title0[ 128 ] = "\t CI-0117 Programación Concurrente\n";
   char title1[ 128 ] = "\t ------------------------------------------------------------\n";
   char title2[ 128 ] = "\t| Process | Atom | Status | H count | O count |\n";
   char title3[ 128 ] = "\t|------------------------------------------------------------|";

   title = newwin( 4, 80, 0, 1 );
   waddstr( title, title0 );
   waddstr( title, title1 );
   waddstr( title, title2 );
   waddstr( title, title3 );
   wrefresh( title );

}

void moleculePrinter(int who, char atom, const char * state) {
    WINDOW * waterWin;
    int posx = 1, posy = (who + 2) * 2;
    char buffer[128];
    char bottomLine[128] = "\t|----------------------------------------------------|\n";
    char lastLine[128]   = "\t ------------------------------------------------------";

    mutex->Lock();
    waterWin = newwin(2, 80, posy, posx);

    sprintf(buffer, "\t|  %5d  |   %c   | %9s |    %2d    |   %2d   |\n",
            getpid(), atom, state, *atomsH, *atomsO);
    waddstr(waterWin, buffer);

    if (who + 1 == TOTAL) {
        waddstr(waterWin, lastLine);
    } else {
        waddstr(waterWin, bottomLine);
    }

    wrefresh(waterWin);

    usleep(200000);

    delwin(waterWin);
    mutex->Unlock();
}

int main( int argc, char *argv[] ) {
    int i, pindex, worker, status;
    pid_t workers[TOTAL];

// curses initialization
    initscr();
    cbreak();
    noecho();
    displayTitle();

    srand(time(NULL));
    safeCount = new Semaforo(1,1);
    bubble   = new Semaforo(1,0);
    mutex    = new Mutex();

    // La memo compartida
    int shmidH = shmget(IPC_PRIVATE, sizeof(int), 0600 | IPC_CREAT);
    int shmidO = shmget(IPC_PRIVATE, sizeof(int), 0600 | IPC_CREAT);
    atomsH = (int*) shmat(shmidH, NULL, 0);
    atomsO = (int*) shmat(shmidO, NULL, 0);
    *atomsH = 0;
    *atomsO = 0;
   
    // Crea procesos H
    for (i = 0; i < NUMH; i++) {
        status = fork();
        if (!status) workerH(i);
        else workers[i] = status;
    }
    // Crea procesos O
    for (i = 0; i < NUMO; i++) {
        status = fork();
        if (!status) workerO(i + NUMH);
        else workers[i + NUMH] = status;
    }

    // Esperar a todos
    for (i = 0; i < TOTAL; i++) {
        worker = waitpid(-1, &status, 0);
        pindex = -1;
        do { pindex++; } while (pindex < TOTAL && worker != workers[pindex]);
        if (-1 == worker) perror("main");
    }

    delete safeCount;
    delete bubble;
    delete mutex;

    shmdt(atomsH);
    shmdt(atomsO);
    shmctl(shmidH, IPC_RMID, NULL);
    shmctl(shmidO, IPC_RMID, NULL);

    echo();
    nocbreak();
    endwin();

    printf("\nNormal end of execution.\n");
    return 0;
}

void workerH( int who ) {
    usleep(200000 + rand() % 800000);

    safeCount->Wait();
    (*atomsH)++;
    moleculePrinter(who, 'H', "Arrived");

    if (*atomsH >= 2 && *atomsO >= 1) {
        // Forma la molecula
        (*atomsH) -= 2;
        (*atomsO) -= 1;
        moleculePrinter(who, 'H', "Forming");
        bubble->Signal(); // Libera un O
        bubble->Signal(); // Libera un H
        bubble->Signal(); // Libera un H
    }
    safeCount->Signal();

    bubble->Wait(); // Espera a que se complete un H2O
    moleculePrinter(who, 'H', "In H2O");

    _exit(0);
}

void workerO( int who ) {
    usleep(200000 + rand() % 800000);

    safeCount->Wait();
    (*atomsO)++;
    moleculePrinter(who, 'O', "Arrived");

    if (*atomsH >= 2 && *atomsO >= 1) {
        // Forma la molecula
        (*atomsH) -= 2;
        (*atomsO) -= 1;
        moleculePrinter(who, 'O', "Forming");
        bubble->Signal(); // Libera un H
        bubble->Signal(); // Libera un H
        bubble->Signal(); // Libera un O
    }
    safeCount->Signal();

    bubble->Wait(); // Espera a que se complete un H2O
    moleculePrinter(who, 'O', "In H2O");

    _exit(0);
}
