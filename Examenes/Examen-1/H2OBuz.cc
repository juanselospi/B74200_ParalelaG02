#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <curses.h>

#include "Buzon.h"
#include "Mutex.h"

#define NUMH 6
#define NUMO 3
#define TOTAL 9

Buzon * buzon;
Mutex * mutex;

int moleculesFormed = 0;

struct MensajeH2O {
    long mtype;
    char atom;
};

void workerH(int who);
void workerO(int who);

void moleculePrinter(int who, char atom, const char * state);

/*
 *  Curses display
 */
void displayTitle() {
   WINDOW * title;
   char title0[ 128 ] = "\t CI-0117 Programación Concurrente\n";
   char title1[ 128 ] = "\t --------------------------------------------------------\n";
   char title2[ 128 ] = "\t| Process | Atom | Status |\n";
   char title3[ 128 ] = "\t|--------------------------------|\n";

   title = newwin( 4, 80, 0, 1 );
   waddstr( title, title0 );
   waddstr( title, title1 );
   waddstr( title, title2 );
   waddstr( title, title3 );
   wrefresh( title );

}

void moleculePrinter( int who, char atom, const char * state ) {
    WINDOW * waterWin;
    int posx = 1, posy = (who + 2) * 2;
    char buffer[128];
    char bottomLine[128] = "\t|-------------------------------------|\n";

    mutex->Lock();
    waterWin = newwin(2, 80, posy, posx);
    sprintf(buffer, "\t| %5d  |   %c   | %9s |\n", getpid(), atom, state);
    waddstr(waterWin, buffer);
    waddstr(waterWin, bottomLine);
    wrefresh(waterWin);

    usleep(200000);

    delwin(waterWin);
    mutex->Unlock();
}

int main() {
    
    pid_t workers[TOTAL];
    int status;
    
    // curses initialization
    initscr();
    cbreak();
    noecho();
    displayTitle();

    srand( time( NULL ) );
    buzon = new Buzon();
    mutex = new Mutex();

    // Crear H
    for (int i = 0; i < NUMH; i++) {
        pid_t pid = fork();
        if (!pid) workerH(i);
        else workers[i] = pid;
    }

    // Crear O
    for (int i = 0; i < NUMO; i++) {
        pid_t pid = fork();
        if (!pid) workerO(i + NUMH);
        else workers[i + NUMH] = pid;
    }

    // Esperar a todo
    for (int i = 0; i < TOTAL; i++) {
        pid_t w = waitpid(-1, &status, 0);
        if (w == -1) perror("waitpid");
    }

    delete mutex;
    delete buzon;
    
    echo();
    nocbreak();
    endwin();

    printf("\nNormal end of execution.\n");

    return 0;
}

void workerH(int who) {
    usleep(100000 + rand() % 400000);
    moleculePrinter(who, 'H', "Arrived");

    // Send H
    MensajeH2O msg;
    msg.mtype = 1;
    msg.atom = 'H';
    buzon->Enviar(&msg, sizeof(msg), 1);

    buzon->Recibir(&msg, sizeof(msg), who + 1);
    moleculePrinter(who, 'H', "In H2O");

    _exit(0);
}

void workerO(int who) {
    usleep(100000 + rand() % 400000);
    moleculePrinter(who, 'O', "Arrived");

    // Send O
    MensajeH2O msg;
    msg.mtype = 2;
    msg.atom = 'O';
    buzon->Enviar(&msg, sizeof(msg), 2);


    int atomsH = 0, atomsO = 0;
    MensajeH2O recv;
    while (1) {
        buzon->Recibir(&recv, sizeof(recv), 0);
        if (recv.atom == 'H') atomsH++;
        if (recv.atom == 'O') atomsO++;

        if (atomsH >= 2 && atomsO >= 1) break;
    }

    // Fusion de aromos en la nueva molecula
    buzon->Enviar(&recv, sizeof(recv), who - 1);
    buzon->Enviar(&recv, sizeof(recv), who - 2);
    buzon->Enviar(&recv, sizeof(recv), who);

    moleculePrinter(who, 'O', "Forming");

    for (int i = 0; i < 3; i++) {
        buzon->Recibir(&recv, sizeof(recv), 0);
    }

    moleculePrinter(who, 'O', "In H2O");

    _exit(0);
}
