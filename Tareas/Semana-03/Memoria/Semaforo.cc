#include "Semaforo.h"

std::vector<int> Semaforo::semIds;

Semaforo::Semaforo( int cantidad, int valorInicial ) {
   int status;
   union semun {
      int              val;
      struct semid_ds *buf;
      unsigned short  *array;
      struct seminfo  *__buf;
   } value;

   this->id = semget( IPC_PRIVATE, cantidad, 0600 | IPC_CREAT | IPC_EXCL );
   this->nsems = cantidad;

   Semaforo::semIds.push_back( this->id );

   value.val = valorInicial;	// Each semaphore will be initialized to this value
   for ( int i = 0; i < this->nsems; i++ ) {
      status = semctl( id, i, SETVAL, value );

      if ( -1 == status ) {
         throw std::runtime_error( "Semaforo::Semaforo( int, int )" );
      }

   }

}

Semaforo::~Semaforo() {

   if(semctl(id, 0, IPC_RMID) == -1) {
      perror("Semaforo::~Semaforo: IPC_RMID failed");
   }

}

int Semaforo::Signal( int cual ) {
   int resultado = -1;

   struct sembuf s;
   {
      /* data */
      s.sem_num = cual;
      s.sem_op = +1;
      s.sem_flg = 0;
   };
   
   resultado = semop(id, &s, 1);

   if ( -1 == resultado ) {
      throw std::runtime_error( "Semaforo::Signal( int )" );
   }
   return resultado;

}

int Semaforo::Wait( int cual ) {
   int resultado = -1;

   struct sembuf w;
   {
      /* data */
      w.sem_num = cual;
      w.sem_op = -1;
      w.sem_flg = 0;
   };
   
   resultado = semop(id, &w, 1);

   if ( -1 == resultado ) {
      throw std::runtime_error( "Semaforo::Wait( int )" );
   }
   return resultado;

}

void Semaforo::SP( int primero, int segundo ) {
   int resultado;
   struct sembuf P[ 2 ];

   P[ 0 ].sem_num = primero;
   P[ 0 ].sem_op  = -1;
   P[ 0 ].sem_flg = 0;

   P[ 1 ].sem_num = segundo;
   P[ 1 ].sem_op  = -1;
   P[ 1 ].sem_flg = 0;

   // resultado = semop( ... );
   resultado = semop(id, P, 2);

   if ( -1 == resultado ) {
      throw std::runtime_error( "Semaforo::SP( int, int )" );
   }

}

void Semaforo::SV( int primero, int segundo ) {
   int resultado;

   // ...
   struct sembuf V[ 2 ];

   V[ 0 ].sem_num = primero;
   V[ 0 ].sem_op = 1;
   V[ 0 ].sem_flg = 0;

   V[ 1 ].sem_num = segundo;
   V[ 1 ].sem_op = 1;
   V[ 1 ].sem_flg = 0;

   resultado = semop(id, V, 2);

   if ( -1 == resultado ) {
      throw std::runtime_error( "Semaforo::SV( int, int )" );
   }
}
