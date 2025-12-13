/**
 *   Representa un conjunto de puntos representado por un vector
 *
**/

#include <math.h>
#include <cassert>
#include "VectorPuntos.h"


/**
 *   Genera numeros random en punto flotante
 *
**/
double randf( double base ) {

   return base * rand() / (RAND_MAX - 1.);

}

/**
 *   Rellena el vector de puntos con puntos definidos en un circulo
 *
**/
// VectorPuntos::VectorPuntos( long cantidad, double radio ) {
//    long punto;
//    double angulo, r, x, y;

//    this->elementos = cantidad;
//    this->bloque = (Punto **) calloc( sizeof( Punto *), cantidad );
//    for ( punto = 0; punto < cantidad; punto++ ) {
//       angulo = randf( 2 * M_PI );
//       r = randf( radio );
//       x = r * cos( angulo );
//       y = r * sin( angulo );
//       this->bloque[ punto ] = new Punto( x, y );
//    }

// }


VectorPuntos::VectorPuntos( long cantidad, double radio ) {
   long punto;
   double angulo, r, x, y, z;

   this->elementos = cantidad;
   this->bloque = (Punto **) calloc( sizeof( Punto *), cantidad );
   for ( punto = 0; punto < cantidad; punto++ ) {
      angulo = randf( 2 * M_PI );
      r = randf( radio );
      x = r * cos( angulo );
      y = r * sin( angulo );
      z = randf( radio );

      this->bloque[ punto ] = new Punto( x, y, z );
   }
}


/**
 *   Rellena el vector de puntos con puntos colocados en el origen
 *
**/
VectorPuntos::VectorPuntos( long cantidad ) {
   long elemento;

   this->elementos = cantidad;
   this->bloque = (Punto **) calloc( sizeof( Punto *), cantidad );
   for ( elemento = 0; elemento < cantidad; elemento++ ) {
      bloque[ elemento ] = new Punto( 0, 0 );
   }

}


/**
 *
**/
VectorPuntos::~VectorPuntos() {
   long elemento;

   for ( elemento = 0; elemento < this->elementos; elemento++ ) {
      delete bloque[ elemento ];
   }

   free( bloque );

}


/**
 *  Retorna el elemento en la posición indicada del vector
 *
**/
Punto * VectorPuntos::operator [] ( long posicion ) {
   assert( posicion >= 0 && posicion < elementos );
   return this->bloque[ posicion ];

}


/**
 *
**/
long VectorPuntos::demeTamano() {

   return this->elementos;

}



/**
 *  De los puntos en nuestro bloque encuentra el que esta más cercano al punto indicado como parámetro
 *
**/
long VectorPuntos::masCercano( Punto * punto ) {
   long elemento, posicion;
   double distancia, minimo;

   minimo = HUGE_VAL;
   posicion = -1;

   for ( elemento = 0; elemento < this->elementos; elemento++ ) {
      distancia = this->bloque[ elemento ]->dist2( punto );
      if ( minimo > distancia ) {
         minimo = distancia;
         posicion = elemento;
      }
   }

   return posicion;

}


/**
 *   Suma de las distancias Euclidianas entre el centro y los elementos de su grupo
 *   @param	centro punto central (media) de cada grupo
 *   @param	clases	vector con los datos de pertenencia a las clases de los puntos
 *
**/
double VectorPuntos::variabilidad( Punto * centro, long clase, long * clases ) {
   long elemento;
   double sum = 0.0;

   for ( elemento = 0; elemento < this->elementos; elemento++ ) {
      if ( clases[ elemento ] == clase ) {
         sum += this->bloque[ elemento ]->dist2( centro );
      }
   }

   return sum;
}


/**
 *   La suma de todas la variabilidades de todos los grupos
 *   @param	muestras	vector con las muestras
 *   @param	clases	vector con los datos de pertenencia a las clases de los puntos
 *
**/
double VectorPuntos::disimilaridad( VectorPuntos * muestras, long * clases ) {
   long clase;
   double sum = 0.0;
   Punto * centro;

   for ( clase = 0; clase < this->elementos; clase++ ) {
      centro = this->bloque[ clase ];
      sum += muestras->variabilidad( centro, clase, clases );
   }

   return sum;

}


/**
 *  Guarda en un archivo la colección de puntos en un imagen, para poder observar el trabajo realizado
 *  Utiliza otro elemento "posiciones" para indicar los centros y hacer las divisiones
 *  Utiliza el formato EPS por que se trata de un archivo de texto con estructura sencilla, aunque no es
 *  recomendado para uso general por problemas de seguridad
 *  @param	centros	vector con los puntos centrales de cada conjunto
 *  @param	posiciones	vector de longitud idéntica a this para determinar a cual clase pertenece el punto
 *  @param	fileName	nombre del archivo para guardar los datos, si existe se sobre-escribe
 *
**/
#define H 400
#define W 400
void VectorPuntos::genEpsFormat( VectorPuntos * centros, long * clases, char * fileName ) {
   long clase, cantidadCentros, pos;
   double minX, maxX, minY, maxY, scale, cx, cy, px, py;
   double * colors;
   FILE * eps;

   cantidadCentros = centros->demeTamano();
   colors = (double *) calloc( sizeof(double) , 3 * cantidadCentros );
 
   for ( clase = 0; clase < cantidadCentros; clase++) {
      pos = 3 * clase;
      colors[ pos + 0 ] = (3 * (clase + 1) % cantidadCentros ) / (double) cantidadCentros;
      colors[ pos + 1 ] = (7 * clase % cantidadCentros ) / (double) cantidadCentros;
      colors[ pos + 2 ] = (9 * clase % cantidadCentros ) / (double) cantidadCentros;
   }
 
   maxX = maxX = -(minY = minY = HUGE_VAL);
   for ( pos = 0; pos < this->elementos; pos++) {
      if ( maxX < this->bloque[ pos ]->demeX() ) {
         maxX = this->bloque[ pos ]->demeX();
      }
      if ( minX > this->bloque[ pos ]->demeX() ) {
         minX = this->bloque[ pos ]->demeX();
      }
      if ( maxY < this->bloque[ pos ]->demeY() ) {
         maxY = this->bloque[ pos ]->demeY();
      }
      if ( minY > this->bloque[ pos ]->demeY() ) {
         minY = this->bloque[ pos ]->demeY();
      }
   }
   scale = W / (maxX - minX);
   if ( scale > H / (maxY - minY) ) {
      scale = H / (maxY - minY);
   }
   cx = (maxX + minX) / 2;
   cy = (maxY + minY) / 2;

   eps = fopen( fileName, "w+" );
   fprintf( eps, "%%!PS-Adobe-3.0\n%%%%BoundingBox: -5 -5 %d %d\n", W + 10, H + 10 );
   fprintf( eps, "/l {rlineto} def /m {rmoveto} def\n"
           "/c { .25 sub exch .25 sub exch .5 0 360 arc fill } def\n"
           "/s { moveto -2 0 m 2 2 l 2 -2 l -2 -2 l closepath "
           "	gsave 1 setgray fill grestore gsave 3 setlinewidth"
           " 1 setgray stroke grestore 0 setgray stroke }def\n"
   );
   for ( clase = 0; clase < cantidadCentros; clase++) {
      fprintf( eps, "%g %g %g setrgbcolor\n", colors[ 3 * clase ], colors[ 3 * clase + 1 ], colors[ 3* clase + 2 ] );
      for ( pos = 0; pos < this->elementos; pos++) {
         if ( clases[ pos ] != clase ) {
            continue;
         }
         px = this->bloque[ pos ]->demeX();
         py = this->bloque[ pos ]->demeY();
         fprintf( eps, "%.3f %.3f c\n", (px - cx) * scale + W / 2, (py - cy) * scale + H / 2 );
      }
// Print the centroid, using a small square
      px = (*centros)[ clase ]->demeX();
      py = (*centros)[ clase ]->demeY();
      fprintf( eps, "\n0 setgray %g %g s\n", (px - cx) * scale + W / 2, (py - cy) * scale + H / 2 );
   }
   fprintf( eps, "\n%%%%EOF" );
   fclose( eps );

   free(colors);

}


/**
 *   Constructor de la copia
 */
VectorPuntos::VectorPuntos( const VectorPuntos &otro ) {
    this->elementos = otro.elementos;
    this->bloque = ( Punto ** ) calloc( sizeof( Punto * ), elementos );

    for( long elemento = 0; elemento < elementos; elemento++ ) {
        Punto *p = otro.bloque[ elemento ];
        // this->bloque[ elemento ] = new Punto( p->demeX(), p->demeY(), 0.0 );
        this->bloque[ elemento ] = new Punto( p->demeX(), p->demeY(), p->demeZ() );

    }
}
