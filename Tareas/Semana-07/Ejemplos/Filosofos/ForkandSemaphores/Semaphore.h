/**
 * Esta clase encapsula las funciones para la utilizacion de semaforos en UNIX
 * Se provee los metodos para realizar la sincronizacion de procesos
 * a traves de los llamados tradicionales a Signal y  Wait
*/

#ifndef _SEM_H
#define _SEM_H

class Semaphore {
   public:
      Semaphore( int = 1 );
      ~Semaphore();
      void Signal();
      void Wait();
      void SP( int, int );
      void SV( int, int );

   private:
      int id, nsems;

};

#endif
