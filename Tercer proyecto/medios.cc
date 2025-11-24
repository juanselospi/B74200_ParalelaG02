/**
 *  Programa base para la construcción de centros en una muestra de datos
 *  La muestra de datos está representada por la variable "puntos", todos los puntos generados al azar
 *  Los centros están representados por la variable "centros", todos los puntos colocados en el origen
 *
 *  CI0117 Programación paralela y concurrente
 *  Tercera tarea programada, grupos 2 y 3
 *  2020-ii
 *
**/


/**
 *  Compilar con:
 *      make
 * 
 *  Esto genera el ejecutable principal: medios.out
 *
 *  Parametros de ejecucion:
 *      ./medios.out [hilos] [puntos] [clases] [archivo.eps] [modo]
 *
 *  Donde:
 *      hilos       -> cantidad de hilos OpenMP a usar (int)
 *      puntos      -> numero de puntos a generar (long)
 *      clases      -> cantidad de clases/centroides (long)
 *      archivo.eps -> nombre del archivo EPS de salida (debe terminar en .eps)
 *      modo        -> 0 = asignacion inicial de puntos a clases por aleatoria
 *                     1 = asignacion inicial de puntos a clases por round robin
 */


#include <cstdio>
#include "VectorPuntos.h"
#include <cstring>
#include <omp.h>
#include <ctime>
#include <vector>

#define HILOS 4
#define PUNTOS 1000000
#define CLASES 17
#define MODO 0

using namespace std;

long totalCambios = 0;	// Contabiliza la totalidad de los cambios realizados al grupo de puntos

/**
 *  Coloca a cada punto en una clase de manera aleatoria
 *  Utiliza el vector de clases para realizar la asignación
 *  
**/
void asignarPuntosAClases( long * clases, int modo, long muestras, long casillas ) {
   long clase, pto;

   switch ( modo ) {
      case 0:	// Aleatorio
         for ( pto = 0; pto < muestras; pto++ ) {
            clase = rand() % casillas;
            clases[ pto ] = clase;
         }
         break;
      case 1:	// A construir por los estudiantes

         // round robin (los puntos se van asignando a los centros uno por uno como cartas repartidas en una mesa de poker)
         for( pto = 0; pto < muestras; pto++ ) {
            clases[ pto ] = pto % casillas;
         }

         break;
   }

}


/**
 *  Recibe los centros, puntos y sus contadores, los reinicia, luego los suma de nuevo y promedia actualizando los centros
 *  
**/
void actualizarCentros( VectorPuntos * centros, VectorPuntos * puntos, long * clases, long * contClases ) {
   long clase, pto;

   // primero reinicio los centros y los contadores
   for( clase = 0; clase < centros->demeTamano(); clase++ ) {
      ( *centros )[ clase ]->ponga( 0, 0, 0 ); // para los ejes x, y, z
      contClases[ clase ] = 0;
   }

   // sumar puntos de cada clase
   for( pto = 0; pto < puntos->demeTamano(); pto++ ) {
      clase = clases[ pto ];
      ( *centros )[ clase ]->sume( ( * puntos )[ pto ] );
      contClases[ clase ]++;
   }

   // calcula el promedio dividiendo la suma de puntos entre su total
   for( clase = 0; clase < centros->demeTamano(); clase++ ) {

      // si existen clases promedio
      if( contClases[ clase ] > 0) {
         ( *centros )[ clase ]->divida( contClases[ clase ] );
      }
   }
}


void actualizarPuntos( VectorPuntos * centros, VectorPuntos * puntos, long * clases, long &cambios ) {
   long pto, actual, nuevo;

   // verifico si para cada punto...
   for( pto = 0; pto < puntos->demeTamano(); pto++ ) {
      actual = clases[ pto ];
      nuevo = centros->masCercano( ( *puntos )[ pto ] );

      // si su centro actual permanece como el mas cerano
      if( actual != nuevo ) {
         clases[ pto ] = nuevo;
         cambios++;
      }
   }
}


// // APPROACH BASADO EN REPARTIR PUNTOS DONDE LOS CENTROS CUALQUIERA LOSP UEDE ACCEDER
void actualizarCentrosOMP_puntos( VectorPuntos *centros, VectorPuntos *puntos, long *clases, long *contClases ) {
   long C = centros->demeTamano();
   long P = puntos->demeTamano();

   // solo 1 hilo reinicia todos los centros a cero
   // para evitar que varios hilos hagan trabajo duplicado
   #pragma omp single
   {
      for( long centro = 0; centro < C; centro++ ) {
         (*centros)[ centro ]->ponga( 0, 0, 0 );
         contClases[ centro ] = 0;
      }
   }

   // cada hilo crea un vector de puntos local que acumula las sumas de puntos por cada clase
   // sumLocal va a guardar la suma de coordenadas de todos los puntos que este hilo esta viendo en la clase C de turno
   vector< Punto > sumLocal( C, Punto( 0, 0, 0 ) );

   // un arreglo local de contadores para saber cuantos puntos son de cada clase en este hilo
   vector< long > countLocal( C, 0 );

   // esto paraleliza el recorrido de todos los puntos y aqui cada hilo en teoria recibe una parte de los puntos
   // este nowait evita que los hilos esperen al final de mi for()
   #pragma omp for schedule( static ) nowait
   for( long pto = 0; pto < P; pto++ ) {
      long clase = clases[ pto ];
      sumLocal[ clase ].sume( (*puntos)[ pto ] );
      countLocal[ clase ]++;
   }

   // zona critica para reduccion = cada hilo combina sus resultados locales con los resultados globales acutles
   #pragma omp critical
   {
      for( long centro = 0; centro < C; centro++ ) {
         (*centros)[ centro ]->sume( &sumLocal[ centro ] );
         contClases[ centro ] += countLocal[ centro ];
      }
   }

   #pragma omp single
   for( long centro = 0; centro < C; centro++ ) {
      if( contClases[ centro ] > 0 ) {
         (*centros)[ centro ]->divida( contClases[ centro ] );
      }
   }
}


void actualizarPuntosOMP_puntos( VectorPuntos *centros, VectorPuntos *puntos, long *clases, long &cambios ) {

   #pragma omp for schedule( static ) reduction( +:cambios ) nowait
   for( long pto = 0; pto < puntos->demeTamano(); pto++ ) {
      long old = clases[ pto ];
      long neu = centros->masCercano( (*puntos)[ pto ] );

      if( old != neu ) {
         clases[ pto ] = neu;
         cambios++;
      }
   }
}


// ESTE ES MI APPROACH NUEVO DONDE LA REPARTICION ES DE LOS CENTROS ENTRE LOS HILOS Y TODOS TIENEN ACCESO A LOS PUNTOS
void actualizarCentrosOMP_clases( VectorPuntos *centros, VectorPuntos *puntos, long *clases, long *contClases ) {
   int tid = omp_get_thread_num();
   int numThreads = omp_get_num_threads();

   long C = centros->demeTamano();
   long P = puntos->demeTamano();

   // division de clases entre hilos
   long clasesPorHilo = C / numThreads;
   long iniClase = tid * clasesPorHilo;
   long finClase;

   if( tid == ( numThreads - 1 )) {
      finClase = C; // el ultimo hilo toma hasta le final

   } else {
      finClase = iniClase + clasesPorHilo; // sino los demas toman su bloque normal de clases
   }

   // cada hilo reinicia solo las clases que le pertenecen
   for( long clase = iniClase; clase < finClase; clase++ ) {
      (*centros)[ clase ]->ponga( 0, 0, 0 );
      contClases[ clase ] = 0;
   }

   // sincronizamos para que todos comiencen desde cero
   #pragma omp barrier

   // defino un chunk manual de puntos
   long chunk = ( P + numThreads - 1 ) / numThreads;
   long iniP = tid * chunk;
   long finP = min( P, iniP + chunk );

   // sumas locales solo para clases que este hilo maneja
   vector< Punto > sumaLocal( C, Punto( 0, 0, 0 ) );
   vector< long > countLocal( C, 0 );

   // cada hilo recorre sus puntos
   for( long punto = iniP; punto < finP; punto++ ) {
      long c = clases[ punto ];
      if( c >= iniClase && c < finClase ) {   // solo toca sus propias clases
         sumaLocal[ c ].sume( (*puntos)[ punto ] );
         countLocal[ c ]++;
      }
   }

   // escribir directamente en los centros globales
   for( long clase = iniClase; clase < finClase; clase++ ) {
      if( countLocal[ clase ] > 0 ) {
         (*centros)[ clase ]->ponga(
            sumaLocal[ clase ].demeX(),
            sumaLocal[ clase ].demeY(),
            sumaLocal[ clase].demeZ()
         );

         contClases[ clase ] = countLocal[ clase ];
      }
   }

   #pragma omp barrier

   for( long clase = iniClase; clase < finClase; clase++ ) {
      if( contClases[ clase ] > 0 ) {
         (*centros)[ clase ]->divida(contClases[ clase ] );
      }
   }
}


void actualizarPuntosOMP_clases( VectorPuntos *centros, VectorPuntos *puntos, long *clases, long &cambios ) {
   int tid = omp_get_thread_num();
   int numThreads = omp_get_num_threads();

   long P = puntos->demeTamano();

   long chunk = ( P + numThreads - 1 ) / numThreads;
   long iniP = tid * chunk;
   long finP = min( P, iniP + chunk );

   long misCambios = 0;

   for( long punto = iniP; punto < finP; punto++ ) {
      long old = clases[ punto ];
      long neu = centros->masCercano( (*puntos)[ punto ] );

      if( old != neu ) {
         clases[ punto ] = neu;
         misCambios++;
      }
   }

   // acumulo cambios con reduccion manual
   #pragma omp atomic
   cambios += misCambios;
}


/**
 *  Programa muestra
 *  Variable: clases, almacena la clase a la que pertenece cada punto, por lo que debe ser del mismo tamaño que las muestras
 *  Variable: contClases, almacena los valores para la cantidad de puntos que pertenecen a un conjunto
**/
int main( int cantidad, char ** parametros ) {
   long cambios, clase, minimo, pto;
   Punto * punto;
   long casillas = CLASES;
   long muestras = PUNTOS;
   const char *paralelaArchivo = "paralela.eps";
   const char *serialArchivo = "ci0117.eps";
   double start, finish, wusedAssign, wusedSerial, wusedParallel; // para tomar los tiempos
   int hilos = HILOS;
   int modo = MODO;

   // input del usuario con validacion de entrada

   if( cantidad > 1 ) { // numero de hilos
      hilos = atol( parametros[ 1 ] );

      if( hilos < 1 ) {
        hilos = HILOS;
        printf( "Cantidad de hilos invalida, usando valor por defecto %d\n", hilos );
      }
   }

   if( cantidad > 2 ) { // para los puntos
      muestras = atol( parametros[ 2 ] );

      if( muestras < 1 ) {
         muestras = PUNTOS;
         printf( "Cantidad de puntos invalida, usando valor por defecto %ld\n", muestras );
      }
   }

   if( cantidad > 3 ) { // para las clases
      casillas = atol( parametros[ 3 ] );

      if( casillas < 1 || casillas > muestras ) {
         casillas = CLASES;
         printf( "Cantidad de clases invalida, usando valor por defecto %ld\n", casillas );
      }
   }

   if( cantidad > 4 ) { // si el usuario le quiere dar un nombre al archivo ( nombres diferentes permiten comparar corridas con diversos parametros de entrada visualmente )
      paralelaArchivo = parametros[ 4 ];

      // comprobar extension .eps correcta
      const char *extencion = strrchr( paralelaArchivo, '.' );

      // si el nombre dle archivo no tiene extencion o no es .eps
      if( extencion == NULL || strcmp( extencion, ".eps" ) != 0 ) {
         printf( "Archivo sin extension .eps, usando ci0117.eps por defecto\n" );
         paralelaArchivo = "ci0117.eps"; // uso el nombre que venia con el .zip del proyecto
      }
   }

   if( cantidad > 5 ) { // para el modo
      modo = atol( parametros[ 5 ] );

      if( modo != 0 && modo != 1 ) {
         modo = MODO;
         printf( "Modo invalido, usando modo por defecto %d\n", modo );
      }

      // printf( "Modo %d\n", modo ); // redundante imprimirlo aqui, lo uso para debuguear
   }

   printf( "Usando %d hilos para generar %ld puntos, para %ld clases -> salida: %s\n", hilos, muestras, casillas, paralelaArchivo );

// Procesar los parámetros del programa

   VectorPuntos * centros = new VectorPuntos( casillas );
   VectorPuntos * puntos  = new VectorPuntos( muestras, 10 );	// Genera un conjunto de puntos limitados a un círculo de radio 10
   // long clases[ muestras ];		// Almacena la clase a la que pertenece cada punto
   // long contClases[ casillas ];
   long *clases      = new long[ muestras ];
   long *contClases  = new long[ casillas ];


   // TIEMPO DE ASIGNACION
   start = omp_get_wtime();

   asignarPuntosAClases( clases, modo, muestras, casillas );	// Asigna los puntos a las clases establecidas

   finish = omp_get_wtime();
   wusedAssign = finish - start;

   printf( "\nTiempo de asignación inicial de puntos (modo %d): %.6f s\n", modo, wusedAssign );


   // COPIAS DE CONSTRUCTOR
   VectorPuntos *centrosOMP = new VectorPuntos( *centros );
   VectorPuntos *puntosOMP  = new VectorPuntos( *puntos );

   long *clasesOMP     = new long[ muestras ];
   long *contClasesOMP = new long[ casillas ];

   for( long punto = 0; punto < muestras; punto++ ) {
      clasesOMP[ punto ] = clases[ punto ];
   }


   // SERIAL
   start = omp_get_wtime();
   
   do {
	// Coloca todos los centros en el origen
	// Promedia los elementos del conjunto para determinar el nuevo centro

      actualizarCentros( centros, puntos, clases, contClases );

      cambios = 0;	// Almacena la cantidad de puntos que cambiaron de conjunto
	// Cambia la clase de cada punto al centro más cercano

      actualizarPuntos( centros, puntos, clases, cambios );

      totalCambios += cambios;

   } while ( cambios > 0 );	// Si no hay cambios el algoritmo converge

   finish = omp_get_wtime();
   wusedSerial = finish - start;

   printf( "\nValor de la disimilaridad en la solución encontrada %g, con un total de %ld cambios\n", centros->disimilaridad( puntos, clases ), totalCambios );
   printf( "Tiempo total de agrupamiento (version serial): %.6f s\n", wusedSerial );

   puntos->genEpsFormat( centros, clases, (char *)serialArchivo );

   

   // EN CASO DE PROBARSE DESCOMENTAR TODO ENTRE LOS // === 

   // // PARALELA (VERSION CON ENFOQUE A DIVIDIR PUNTOS ENTRE HILOS Y ACCEDER A CENTROS DE FORMA CONCURRENTE)
   // =========================================================================

   // totalCambios = 0;

   // start = omp_get_wtime();

   // #pragma omp parallel num_threads( hilos ) shared( centrosOMP, puntosOMP, clasesOMP, contClasesOMP, totalCambios )
   // {
   //    do {
   //       // Coloca todos los centros en el origen
   //       // Promedia los elementos del conjunto para determinar el nuevo centro
   //       actualizarCentrosOMP_puntos( centrosOMP, puntosOMP, clasesOMP, contClasesOMP );
         
   //       #pragma omp single
   //       cambios = 0;
         
   //       actualizarPuntosOMP_puntos( centrosOMP, puntosOMP, clasesOMP, cambios );

   //       #pragma omp barrier

   //       #pragma omp single
   //       totalCambios += cambios;

   //    } while ( cambios > 0 ); // Si no hay cambios el algoritmo converge
   // }

   // finish = omp_get_wtime();
   // wusedParallel = finish - start;


   // ==========================================================================



   // EN CASO DE QUERER PROBAR LA VERSION DE ARRIBA COMENTAR TODO ENTRE LOS // .......

   // PARALELA (VERSION ACTUAL = DIVIDO LOS CENTROS ENTRE LOS HILOS HILOS Y ACCESO CONCURRENTE A LOS PUNTOS)
   // ..........................................................................

   totalCambios = 0;

   start = omp_get_wtime();

   #pragma omp parallel num_threads( hilos ) shared( centrosOMP, puntosOMP, clasesOMP, contClasesOMP, totalCambios )
   {
      do {
         actualizarCentrosOMP_clases( centrosOMP, puntosOMP, clasesOMP, contClasesOMP );

         #pragma omp single
         cambios = 0;

         actualizarPuntosOMP_clases( centrosOMP, puntosOMP, clasesOMP, cambios );

         #pragma omp barrier

         #pragma omp single
         totalCambios += cambios;

      } while( cambios > 0 );
   }

   finish = omp_get_wtime();
   wusedParallel = finish - start;

   // .............................................................................

   printf( "\nValor de la disimilaridad en la solución encontrada %g, con un total de %ld cambios\n", centrosOMP->disimilaridad( puntosOMP, clasesOMP ), totalCambios );
   printf( "Tiempo total de agrupamiento (version paralela): %.6f s\n", wusedParallel );

   double speedUp = wusedSerial / wusedParallel;
   printf( "\nSpeedUp: %.4fx\n", speedUp );

// Con los valores encontrados genera el archivo para visualizar los resultados
   puntosOMP->genEpsFormat( centrosOMP, clasesOMP, (char *)paralelaArchivo );

   delete[] clases;
   delete[] contClases;
   delete[] clasesOMP;
   delete[] contClasesOMP;
   delete puntos;
   delete centros;
   delete puntosOMP;
   delete centrosOMP;
}
