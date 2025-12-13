/**
 * Represent a shopping store
 *
 * @author	Programacion Concurrente
 * @version	2025/Oct/10
 *
 * Primer examen parcial
 *
 * Grupo 3
 *
**/


#include <string>		// std::string
#include <unistd.h>		// calloc


#include "tienda.h"

struct shoppingCart
{
    // datos que ya estaban
    int articulos;
    int capacidadCarrito;
    int *pesos;
    int *precios;

    // datos que agregue en mi forma serial porque los considere utiles
    int *canasta;
    int highestProfit;
    int optimalCapacity;

    // para hilos si indice de inicio y final para que solo tomen una seccion de trabajo ccada uno
    int start;
    int finish;
};


/**
 * Default contructor
 * Create a store with random filled values for prices and weights
 *
**/
Tienda::Tienda( int items, int capacidadCarrito ) {
   int item;

   srand( getpid() );
 
   this->articulos = items;
   this->capacidadCarrito = capacidadCarrito;
   this->pesos   = (int *) calloc( items, sizeof( int ) );
   this->precios = (int *) calloc( items, sizeof( int ) );
   for ( item = 0; item < items; item++ ) {
      this->pesos[ item ] = 1 + (random() & 15);
      this->precios[ item ] = 10 + (random() & 15);
   }

}


/**
 * Parameter constructor
 * @param	int	items quantity
 * @param	int *	array with items' weights
 * @param	int *	array with items' values
 *
**/
Tienda::Tienda( int capacidadCarrito, int items, const int * pesos, const int * precios ) {
   int item;

   this->capacidadCarrito = capacidadCarrito;
   this->articulos = items;
   this->pesos   = (int *) calloc( items, sizeof( int ) );
   this->precios = (int *) calloc( items, sizeof( int ) );
   for ( item = 0; item < items; item++ ) {
      this->pesos[ item ] = pesos[ item ];
      this->precios[ item ] = precios[ item ];
   }

}


/**
 *
 *
**/
Tienda::~Tienda() {

   free( this->pesos );
   free( this->precios );

}


/**
 * Return a String with store items
**/
std::string Tienda::toString() {
   std::string result = "";
   int item;

   result += "Tienda con " + std::to_string( this->articulos ) + " artículos y capacidad de carrito " 
          + std::to_string( this->capacidadCarrito ) + "\n";
   result += "Pesos = { ";
   for ( item = 0; item < this->articulos; item++ ) {
      result += std::to_string( this->pesos[ item ] );
      if ( item < this->articulos - 1 ) {
         result += ", ";
      }
   }

   result += " }\nPrecios = { ";
   for ( item = 0; item < articulos; item++ ) {
      result += std::to_string( this->precios[ item ] );
      if ( item < this->articulos - 1 ) {
         result += ", ";
      }
   }
   result += " }\n";

   return result;

}


/**
 *
 *   Coloca artículos en el carrito para maximizar ganancias sin pasarse de un peso establecido
 *
 **/
void Tienda::llenarCarrito() {

    // canasta generica para ordenar por precios
    int canasta [this->articulos];

    // variables auxiliares
    int priciest = 0;
    int remainingCapacity;
    int optimalCapacity;
    int currentProfit = 0;
    int highestProfit = 0;

    // ordenar la canasta
    for( int item = 0; item < this->articulos; item++ ) {
        
        for( int item2 = 0; item2 < this->articulos; item2++ ) {

            if( precios[ item ] > precios[ item2 ]) {

                priciest = item;
            } else if( precios[ item ] < item2 ) {

                priciest = item2;
            } else {

                priciest = item;
            }
        }
        
        // termina como un arreglo que guarda el indice de los articulos ordenado de mas caro al mas barato
        canasta[ item ] = priciest;
    }


   // por cada articulo que tengo, desde el mas caro al mas barato
   for( int item = 0; item < this->articulos; item++ ) {
        
        remainingCapacity = this->capacidadCarrito;
        currentProfit = 0;

        // para el primer item de cada version del carrito verifico que su peso no exeda el limite del carrito y lo agrego
        if( pesos[ canasta[ item ] ] <= remainingCapacity ) {

            remainingCapacity = this->capacidadCarrito - pesos[ canasta[ item ] ];
            currentProfit = currentProfit + precios[ canasta[ item ] ];

            // con el articulo mas caro dentro del carrito verifico los articulos restantes y meto los que pueda hasta llenar mi capacidad o agotar existencias
            for( int item2 = 0; item2 < this->articulos; item2++ ) {
                
                if(item2 == item){

                    // no pasa nada porque no se pueden repetir articulos
                } else {

                    // para cada articulo que encuentro que pueda entrar al carrito le sumo su precio a la ganancia y substrigo su peso de la capacidad restante
                    if( pesos[ canasta[ item2 ] ] <= remainingCapacity ){

                        remainingCapacity = remainingCapacity - pesos[ canasta[ item2 ] ];
                        currentProfit = currentProfit + precios[ canasta[ item2] ];
                    }

                }
            }

            // si la ganancia actual es mayor que la ganancia maxima actualizo la ganancia maxima
            if( currentProfit > highestProfit) {
                highestProfit = currentProfit;

                // y tambien actualizo la capacidad que utilice que seria la optima
                optimalCapacity = this->capacidadCarrito - remainingCapacity;

            }


        }  else if( pesos[ canasta[ item ] ] > remainingCapacity ) {
            printf("Este item es demasiado pesado para el carrito\n");

        }
   }

   // imprimo los resultados obtenidos
   printf( "La ganancia total es de: [ %d ], usando [ %d ] kilos de capacidad\n", highestProfit, optimalCapacity );

}

void *worker( void *args ) {

    //convierto el puntero generico void en uno tipo shoppingCart para acceder a los datos
    shoppingCart *groceries = ( shoppingCart * )args;

    int highestProfit = 0;
    int optimalCapacity = 0;

    // cada hilo va a procesar una parte de los articulos disponibles
    for ( int item = groceries->start; item < groceries->finish; item++ ) {

        // da la capacidad al carrito de esta iteracion
        int remainingCapacity = groceries->capacidadCarrito;
        int currentProfit = 0;

         // agrego el articulo inicial si cabe en el carrito
        if ( groceries->pesos[ groceries->canasta[ item ] ] <= remainingCapacity ) {

            remainingCapacity -= groceries->pesos[ groceries->canasta[ item ] ];
            currentProfit += groceries->precios[ groceries->canasta[ item ] ];

            // si puede le hilo intentara agregar mas articulos al carrito
            for ( int item2 = 0; item2 < groceries->articulos; item2++ ) {
                if ( item2 == item ) {
                    // no pasa nada porque no se puede repetir items
                }

                // lo agrega si cabe
                if ( groceries->pesos[ groceries->canasta[ item2 ] ] <= remainingCapacity ) {

                    remainingCapacity -= groceries->pesos[ groceries->canasta[ item2 ] ];
                    currentProfit += groceries->precios[ groceries->canasta[ item2 ] ];
                }
            }

            // al terminar de cargar mis carritos queda el mejor profit
            if ( currentProfit > highestProfit ) {
                highestProfit = currentProfit;
                optimalCapacity = groceries->capacidadCarrito - remainingCapacity;
            }
        }
    }

    // esto guarda en la estrcutura los mejores resultados obtenidos
    groceries->highestProfit = highestProfit;
    groceries->optimalCapacity = optimalCapacity;

    // finalizo el hilo
    pthread_exit(NULL);
}






void Tienda::llenarCarritoPthreads( int familyMembers ) {


    int *canasta = ( int * )calloc( this->articulos, sizeof( int ) );

    // ordenar la canasta por precios como en la version serial
    for ( int i = 0; i < this->articulos; i++ ) {

        // mi articulo mas caro hasta el momento
        int priciest = 0;

        // comparo precios y actualizo el mas caro
        for ( int j = 0; j < this->articulos; j++ ) {
            
            if ( precios[ i ] > precios[ j ] ) {
                
                priciest = i;
            } else {

                priciest = j;
            }

        }

        // meto el mas caro en mi indice de canasta actual
        canasta[ i ] = priciest;
    }

    // agrego la cantidad de hilos que desee asignar
    pthread_t threads[ familyMembers ];
    shoppingCart args[ familyMembers ];

    // divido en partes iguales el trabajo entre los hilos
    int chunk = this->articulos / familyMembers;
    int resto = this->articulos % familyMembers;


    int start = 0;

    // a cada hilo de la familia lo mando a trabajar en una seccion de articulos diviendo el trabajo
    for ( int i = 0; i < familyMembers; i++ ) {

        int finish = 0;

        // si es numero primo el indice tiene 1 demas
        if( i < resto ) {
            
            finish = start + chunk + 1;
        } else {
            finish = start + chunk;
        }

        args[ i ].articulos = this->articulos;
        args[ i ].capacidadCarrito = this->capacidadCarrito;
        args[ i ].pesos = this->pesos;
        args[ i ].precios = this->precios;

        args[ i ].canasta = canasta;
        args[ i ].highestProfit = 0;
        args[ i ].optimalCapacity = 0;

        args[ i ].start = start;
        args[ i ].finish = finish;

        // pone os hilos a trabajar
        pthread_create( &threads[ i ], NULL, worker, ( void * )&args[ i ] );

        // el inicio del indice del siguiente hilo es el final de este
        start = finish;

    }

    int globalBest = 0;
    int globalOptimalCapacity = 0;

    // al tener los resultados de todos los hilos los compara
    for ( int i = 0; i < familyMembers; i++ ) {

        // espero a los hilos que estan trabajando
        pthread_join( threads[ i ], NULL );

        // si hay una gnancia mayor a la ganancia actual se actualiza el top global
        if ( args[ i ].highestProfit > globalBest ) {

            globalBest = args[ i ].highestProfit;
            globalOptimalCapacity = args[ i ].optimalCapacity;
        }
    }

    printf( "La ganancia total usando pthreads es de: [ %d ], usando [ %d ] kilos de capacidad\n", globalBest, globalOptimalCapacity );

    free( canasta );

}
