/**
 *   Representa un conjunto de puntos en dos dimensiones
 *
**/


#include "Punto.h"


class VectorPuntos {
   public:
      VectorPuntos( long, double );
      VectorPuntos( long );
      ~VectorPuntos();
      long demeTamano();
      long masCercano( Punto * );
      double variabilidad( Punto *, long, long * );
      double disimilaridad( VectorPuntos *, long * );
      void genEpsFormat( VectorPuntos *, long *, const char * );
      Punto * operator [] ( long );
      VectorPuntos(const VectorPuntos &otro);  // constructor de la copia


   private:
      long elementos;
      Punto ** bloque;

};

