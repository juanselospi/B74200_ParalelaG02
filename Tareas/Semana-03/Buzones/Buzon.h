/**
  *  C++ class to encapsulate Unix message passing intrinsic structures and system calls
  *
  *  Author: Programacion Concurrente (Francisco Arroyo)
  *  Version: 2025/Ago/26
  *
 **/

#include <unistd.h>	// pid_t definition
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>

#define KEY 0xA12345	// Valor de la llave del recurso
#define LABEL_SIZE 64

struct Mensaje
{
   long mtype;
   long times;
   char label[ LABEL_SIZE ];
};


class Buzon {
   public:
      Buzon();
      ~Buzon();
      ssize_t Enviar( const char *label, long counter, long type );
      ssize_t Enviar( const void *buffer, size_t len, long = 1 );	// Send a message (len sized) pointed by buffer to mailbox
      ssize_t Recibir( void *buffer, size_t len, long = 1 );	// size_t: space in buffer

      void Eliminar();

   private:
      int id;		// Identificador del buzon
      pid_t owner;	// Dueño del recurso
};

