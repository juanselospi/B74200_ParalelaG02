/**
  *  C++ class to encapsulate Unix message passing intrinsic structures and system calls
  *
  *  Author: Programacion Concurrente (Francisco Arroyo)
  *  Version: 2025/Ago/26
  *
 **/
#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stddef.h>  // para size_t
#include <unistd.h>	// pid_t definition
#define KEY 0xA12345	// Valor de la llave del recurso
#define LABEL_SIZE 64

struct Mensaje
{
    long mtype;
    long times;
    char label[LABEL_SIZE];
};

class Buzon {
   public:
      Buzon();
      ~Buzon();
      ssize_t Enviar( const char *label, long contador, long tipo);
      ssize_t Enviar( const void *buffer, size_t len, long = 1 );	// Send a message (len sized) pointed by buffer to mailbox
      ssize_t Recibir( void *buffer, size_t len, long = 1 );	// size_t: space in buffer

   private:
      int id;		// Identificador del buzon
      pid_t owner;	// Dueño del recurso
};
