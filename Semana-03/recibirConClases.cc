/**
  *  C++ program to receive messages via operating system message passing queues
  *
  *  Author: Programacion Concurrente (Francisco Arroyo)
  *
  *  Version: 2025/Ago/26
  *
 **/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Buzon.h"

#define LABEL_SIZE 64


int main( int argc, char ** argv ) {


// struct Mensaje {
//        long mtype;     // message type, must be > 0 
//        int times;	// Times that label appears
//        char label[ LABEL_SIZE ];  // Label to send to mailbox
// };

   Mensaje A;
   ssize_t st;

//    int id, size, st;
   Buzon m;

   st = m.Recibir( (void *) &A, sizeof(A) - sizeof(long), 2025 );  // Receives a message with 2019 type
   while ( st > 0 ) {
      printf("Label: %s, times %d \n", A.label, A.times );
      st = m.Recibir( (void *) &A, sizeof(A) - sizeof(long), 2025 );
   }

}
