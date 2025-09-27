/*  Esta clase encapsula las funciones para la utilizacion de semáforos
 *  con la modalidad de que se pueden n semáforos simultáneamente.
 *  Algunos problemas, como el de los filósofos, requieren de esta funcionalidad
 *  SP y SV van a solicitar acceso a dos semáforos de manera simultánea
 *
 *  Autor: Programacion Paralela y Concurrente
 *  Fecha: 2020/May/16
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "Semaphore.h"

    union semun {
       int              val;
       struct semid_ds *buf;
       unsigned short  *array;
       struct seminfo  *__buf;
    };


/**
 *  Constructs an array of n Unix semaphores initialized to one
 * @param	int	semaphore quantity
 *
 */
Semaphore::Semaphore( int n ) {
   int status;
   union semun value;

   this->id = semget( IPC_PRIVATE, n, 0600 | IPC_CREAT | IPC_EXCL );
   this->nsems = n;

   value.val = 1;	// Each semaphore will be initialized to one 
   for ( int i = 0; i < this->nsems; i++ ) {
       status = semctl( id, i, SETVAL, value );
       if ( -1 == status ) {
          perror( "Semaphore::Semaphore(int)" );
       }
   }

}


/**
 * Destroys the semaphore array
 */
Semaphore::~Semaphore() {
   int status;

   status = semctl( id, 0, IPC_RMID, NULL );
   if ( -1 == status ) {
      perror( "Semaphore::~Semaphore" );
   }

}


/**
 * Signal operation on semaphore array, will try to add one to each semaphore in the array
 * Method will return until it is possible to modify each element in the array
 *
 */
void Semaphore::Signal() {
   int status;
   struct sembuf V[nsems];

   for ( int i = 0; i < nsems; i++ ) {
      V[i].sem_num = i;
      V[i].sem_op  = 1;
      V[i].sem_flg = 0;
   }
   status = semop( id, V, nsems );
   if ( -1 == status ) {
      perror( "Semaphore::Semaphore(int, int)" );
   }
} 

  
/**
 * Wait operation on semaphore array, will try to substract one to each semaphore in the array
 * Method will return until it is possible to modify each element in the array
 *
 */
void Semaphore::Wait() {
   int status;
   struct sembuf P[ this->nsems ];

   for ( int i = 0; i < this->nsems; i++ ) {
      P[i].sem_num = i;
      P[i].sem_op  = -1;
      P[i].sem_flg = 0;
   }
   status = semop( id, P, nsems );
   if ( -1 == status ) {
      perror( "Semaphore::Semaphore(int, int)" );
   }

}



/**
 * Do a Wait operation on two semaphores, will try to substract one to each semaphore
 * This operation must be atomic, not allowed to block one semaphore and try to block the other
 * The block operation must occur on both semaphores atomically
 * It will try to get the lock on two semaphores from the group, especified by parameters
 *
 */
void Semaphore::SP( int first, int second ) {
   int status;
   struct sembuf P[ 2 ];

   P[ 0 ].sem_num = first;
   P[ 0 ].sem_op  = -1;
   P[ 0 ].sem_flg = 0;

   P[ 1 ].sem_num = second;
   P[ 1 ].sem_op  = -1;
   P[ 1 ].sem_flg = 0;

   status = semop( this->id, P, 2 );
   if ( -1 == status ) {
      perror( "Semaphore::SP(int, int)" );
   }
}


/**
 * Do a Signal operation on two semaphores, will try to substract one to each semaphore
 * This operation must be atomic, not allowed to block one semaphore and try to block the other
 * The free operation must occur on both semaphores atomically
 */
void Semaphore::SV( int first, int second ) {
   int status;
   struct sembuf V[ 2 ];

   V[ 0 ].sem_num = first;
   V[ 0 ].sem_op  = 1;
   V[ 0 ].sem_flg = 0;

   V[ 1 ].sem_num = second;
   V[ 1 ].sem_op  = 1;
   V[ 1 ].sem_flg = 0;

   status = semop( this->id, V, 2 );
   if ( -1 == status ) {
      perror( "Semaphore::SV(int, int)" );
   }
}

