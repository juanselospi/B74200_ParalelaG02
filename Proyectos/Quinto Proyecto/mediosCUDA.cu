/**
 *  Compilar con:
 *      make
 * 
 *  Esto genera el ejecutable principal: medios.out
 *
 *  Parametros de ejecucion:
 *      ./mediosCUDA.out [bloque CUDA(preferiblemente 128, 256 o 512 segun documentacion de cuda)] [puntos] [clases] [archivo.eps] [modo]
 *
 *  Donde:
 *      bloque CUDA  -> cantidad de hilos por bloque CUDA
 *      puntos      -> numero de puntos a generar (long)
 *      clases      -> cantidad de clases/centroides (long)
 *      archivo.eps -> nombre del archivo EPS de salida (debe terminar en .eps)
 *      modo        -> 0 = asignacion inicial de puntos a clases por aleatoria
 *                     1 = asignacion inicial de puntos a clases por round robin
 */
#ifdef __INTELLISENSE__
#define __CUDACC__
#endif

#include <cstdio>
#include "VectorPuntos.h"
#include <cstring>
#include <omp.h>
#include <ctime>
#include <vector>
#include <cstdlib>


// CUDA
#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>


#define CUDA 256
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

__global__
void actualizarPuntosCUDA( double *puntosX, double *puntosY, double *puntosZ, double *centrosX, double *centrosY, double *centrosZ, long *clases, long casillas, long muestras, int *cambiosKernel ) {
   long pto = blockIdx.x * blockDim.x + threadIdx.x;

   if( pto >= muestras ) {
      return;
   }

   // mi punto actual
   double ptoX = puntosX[ pto ];
   double ptoY = puntosY[ pto ];
   double ptoZ = puntosZ[ pto ];

   long claseActual = clases[ pto ];


   // EXPLICACION: CUDA no soporta llamadas a metodos de C complejos, no esta hecho para orientacion a ojetos, funciona con arreglos planos y aritmetica sencilla
   // por lo que todas las funciones de la parte CUDA deben estar en este archivo.cu, por eso agregue un equivalente a masCercano() aqui porque el original no fue soportado por CUDA

   // busca el centro mas cercano
   double mejorDist = 1e30;
   long mejorClase = claseActual;

   for( long clase = 0; clase < casillas; clase++ ) {

      double dx = ptoX - centrosX[ clase ];
      double dy = ptoY - centrosY[ clase ];
      double dz = ptoZ - centrosZ[ clase ];

      double dist = dx * dx + dy * dy + dz * dz;

      if( dist < mejorDist ) {

         mejorDist = dist;
         mejorClase = clase;
      }
   }

   // si el punto cambio de clase
   if( mejorClase != claseActual ) {

      clases[ pto ] = mejorClase;
      atomicAdd( cambiosKernel, 1 );   // acumulo los cambios en la dGPU de nvida
   }
}


/**
 *  Programa muestra
 *  Variable: clases, almacena la clase a la que pertenece cada punto, por lo que debe ser del mismo tamaño que las muestras
 *  Variable: contClases, almacena los valores para la cantidad de puntos que pertenecen a un conjunto
**/
int main( int cantidad, char ** parametros ) {
   long cambios;
   long casillas = CLASES;
   long muestras = PUNTOS;
   const char *cudaArchivo = "cuda.eps";
   const char *serialArchivo = "ci0117.eps";
   double start, finish, wusedAssign, wusedSerial, wusedCUDA; // para tomar los tiempos
   int cuda = CUDA;
   int modo = MODO;

   // input del usuario con validacion de entrada

   if( cantidad > 1 ) {
      cuda = atol( parametros[ 1 ] );

      if( cuda < 1 ) {
        cuda = CUDA;
        printf( "Cantidad de CUDA invalida, usando valor por defecto %d\n", cuda );
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
      cudaArchivo = parametros[ 4 ];

      // comprobar extension .eps correcta
      const char *extencion = strrchr( cudaArchivo, '.' );

      // si el nombre dle archivo no tiene extencion o no es .eps
      if( extencion == NULL || strcmp( extencion, ".eps" ) != 0 ) {
         printf( "Archivo sin extension .eps, usando cuda.eps por defecto\n" );
         cudaArchivo = "cuda.eps"; // uso el nombre que venia con el .zip del proyecto
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

   printf( "Usando %d CUDA para generar %ld puntos, para %ld clases -> salida: %s\n", cuda, muestras, casillas, cudaArchivo );

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
   VectorPuntos *centrosCUDA = new VectorPuntos( *centros );
   VectorPuntos *puntosCUDA  = new VectorPuntos( *puntos );

   long *clasesCUDA     = new long[ muestras ];
   long *contclasesCUDA = new long[ casillas ];

   for( long punto = 0; punto < muestras; punto++ ) {
      clasesCUDA[ punto ] = clases[ punto ];
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

   puntos->genEpsFormat( centros, clases, serialArchivo );

   // CUDA
   totalCambios = 0;
   int cambiosCUDA = 0;

   // arreglos planos para enviar a la dGPU
   double *puntosCPUX = new double[ muestras ];
   double *puntosCPUY = new double[ muestras ];
   double *puntosCPUZ = new double[ muestras ];

   double *centrosCPUX = new double[ casillas ];
   double *centrosCPUY = new double[ casillas ];
   double *centrosCPUZ = new double[ casillas ];

   for( long punto = 0; punto < muestras; punto++ ) {

      Punto *pto = ( *puntosCUDA )[ punto ];

      puntosCPUX[ punto ] = pto->demeX();
      puntosCPUY[ punto ] = pto->demeY();
      puntosCPUZ[ punto ] = pto->demeZ();
   }

   for( long centro = 0; centro < casillas; centro++ ) {

      Punto *clase = (*centrosCUDA)[ centro ];

      centrosCPUX[ centro ] = clase->demeX();
      centrosCPUY[ centro ] = clase->demeY();
      centrosCPUZ[ centro ] = clase->demeZ();
   }

   // puntos en dGPU
   double *dPuntosX, *dPuntosY, *dPuntosZ;
   cudaMalloc( &dPuntosX, muestras * sizeof( double ));
   cudaMalloc( &dPuntosY, muestras * sizeof( double ));
   cudaMalloc( &dPuntosZ, muestras * sizeof( double ));

   // centros en dGPU
   double *dCentrosX, *dCentrosY, *dCentrosZ;
   cudaMalloc( &dCentrosX, casillas * sizeof( double ));
   cudaMalloc( &dCentrosY, casillas * sizeof( double ));
   cudaMalloc( &dCentrosZ, casillas * sizeof( double ));

   // clases de cada punto en la dGPU
   long *dClases;
   cudaMalloc( &dClases, muestras * sizeof( long ));

   // copia de los puntos a la memoria de la dGPU
   cudaMemcpy( dPuntosX, puntosCPUX, muestras * sizeof( double ), cudaMemcpyHostToDevice );
   cudaMemcpy( dPuntosY, puntosCPUY, muestras * sizeof( double ), cudaMemcpyHostToDevice );
   cudaMemcpy( dPuntosZ, puntosCPUZ, muestras * sizeof( double ), cudaMemcpyHostToDevice );

   // copiar centros igual
   cudaMemcpy( dCentrosX, centrosCPUX, casillas * sizeof( double ), cudaMemcpyHostToDevice );
   cudaMemcpy( dCentrosY, centrosCPUY, casillas * sizeof( double ), cudaMemcpyHostToDevice );
   cudaMemcpy( dCentrosZ, centrosCPUZ, casillas * sizeof( double ), cudaMemcpyHostToDevice );

   // copiar clases iniciales
   cudaMemcpy( dClases, clasesCUDA, muestras * sizeof( long ), cudaMemcpyHostToDevice );


   // reserva de memoria para contador de cambios en la dGPU
   int *dCambios;
   cudaMalloc( &dCambios, sizeof( int ));


   start = omp_get_wtime();

   do {
      // reinicia el contador en la dGPU
      cudaMemset(dCambios, 0, sizeof(int));

      // recalculo la clase de cada punto
      int blockSize = cuda; // tamaño de cada bloque de CUDA, segun la documentacion tipicamente se usa entre 128, 256, 52
      int gridSize = ( muestras + blockSize - 1 ) / blockSize;

      actualizarPuntosCUDA<<<gridSize, blockSize>>>( dPuntosX, dPuntosY, dPuntosZ, dCentrosX, dCentrosY, dCentrosZ, dClases, casillas, muestras, dCambios );

      cudaDeviceSynchronize();

      // copia los resultados de la dGPU con cuda a la CPU
      cudaMemcpy( &cambiosCUDA, dCambios, sizeof( int ), cudaMemcpyDeviceToHost );

      // actualiza los cambios
      cambios = cambiosCUDA;
      totalCambios += cambiosCUDA;

      cudaMemcpy( clasesCUDA, dClases, muestras * sizeof( long ), cudaMemcpyDeviceToHost );

      // recalcula los centros igual que en serial
      actualizarCentros( centrosCUDA, puntosCUDA, clasesCUDA, contclasesCUDA );

      // actualiza los centros en al dGPU
      for( long centro = 0; centro < casillas; centro++ ) {

         centrosCPUX[ centro ] = (*centrosCUDA)[ centro ]->demeX();
         centrosCPUY[ centro ] = (*centrosCUDA)[ centro ]->demeY();
         centrosCPUZ[ centro ] = (*centrosCUDA)[ centro ]->demeZ();
      }

      cudaMemcpy( dCentrosX, centrosCPUX, casillas * sizeof( double ), cudaMemcpyHostToDevice );
      cudaMemcpy( dCentrosY, centrosCPUY, casillas * sizeof( double ), cudaMemcpyHostToDevice );
      cudaMemcpy( dCentrosZ, centrosCPUZ, casillas * sizeof( double ), cudaMemcpyHostToDevice );

   } while ( cambios > 0 );

   finish = omp_get_wtime();
   wusedCUDA = finish - start;

   printf( "\nValor de la disimilaridad en la solución encontrada %g, con un total de %ld cambios\n", centrosCUDA->disimilaridad( puntosCUDA, clasesCUDA ), totalCambios );
   printf( "Tiempo total de agrupamiento (version CUDA): %.6f s\n", wusedCUDA );


   double speedUp = wusedSerial / wusedCUDA;
   printf( "\nSpeedUp: %.4fx\n", speedUp );

// Con los valores encontrados genera el archivo para visualizar los resultados
   puntosCUDA->genEpsFormat( centrosCUDA, clasesCUDA, cudaArchivo );

   delete[] clases;
   delete[] contClases;
   delete[] clasesCUDA;
   delete[] contclasesCUDA;
   delete puntos;
   delete centros;
   delete puntosCUDA;
   delete centrosCUDA;

   cudaFree( dPuntosX );
   cudaFree( dPuntosY );
   cudaFree( dPuntosZ );
   cudaFree( dCentrosX );
   cudaFree( dCentrosY );
   cudaFree( dCentrosZ );
   cudaFree( dClases );
   cudaFree( dCambios );

   delete[] puntosCPUX;
   delete[] puntosCPUY;
   delete[] puntosCPUZ;
   delete[] centrosCPUX;
   delete[] centrosCPUY;
   delete[] centrosCPUZ;
}


// PARA EL QUE LO REVISA

/*
Corriendo con parametros default el programa da un output como este:

Usando 256 CUDA para generar 100000 puntos, para 17 clases -> salida: ci0117.eps

Tiempo de asignación inicial de puntos (modo 0): 0.001071 s

Valor de la disimilaridad en la solución encontrada 730357, con un total de 271693 cambios
Tiempo total de agrupamiento (version serial): 3.055658 s

Valor de la disimilaridad en la solución encontrada 728739, con un total de 486132 cambios
Tiempo total de agrupamiento (version CUDA): 0.471964 s

SpeedUp: 6.4744x

CONCLUSION: el speedUp es bastante notable a comparacion de usar OpenMP, sin embargo CUDA se ve limitado a usar estructuras menos complejas como arrays planos
            y no ser compatible con la orienctacion a objetos que usamos tipicamente.
            Sin embargo la ganancia es enorme a costa de un mayor ruido en la imagen de salida que ya es un contra endemico de usar CUDA.
*/