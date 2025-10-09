/**
  *  C++ program to send messages via operating system message passing queues
  *
  *  No C++ class implementation
  *
  *  Author: Programacion Concurrente (Francisco Arroyo)
  *
  *  Version: 2025/Ago/21
  *
 **/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define KEY 0xA12345
#define LABEL_SIZE 64

const char * html_labels[] = {
   "a",
   "b",
   "c",
   "d",
   "e",
   "li",
   ""
};

int main( int argc, char ** argv ) {


struct msgbuf {
       long mtype;     // message type, must be > 0 
       int times;	// Times that label appears
       char label[ LABEL_SIZE ];  // Label to send to mailbox
};

   struct msgbuf A;
   int id, i, size, st;

   id = msgget( KEY, 0600 | IPC_CREAT );
   if ( -1 == id ) {
      perror("t0-envia: ");
      exit(1);
   }

   A.mtype = 2025;
   i = 0;

   while ( strlen(html_labels[ i ] ) ) {
      A.times = i;
      strcpy( A.label, html_labels[ i ] );
      size = sizeof( A );
      st = msgsnd( id,  &A, size, IPC_NOWAIT );
      printf("Label: %s, status %d \n", html_labels[ i ], st );
      i++;
   }


/*
   msgctl( id, IPC_RMID, NULL );
*/

}

