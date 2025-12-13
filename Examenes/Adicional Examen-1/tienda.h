/**
 * Represent a shopping store
 *
 * @author	Programacion Concurrente
 *
 * @version	2025/Oct/10
 *
 * Primer examen parcial
 *
 * Grupo 3
 *
**/
#include <iostream>
#include <pthread.h>

class Tienda {
   public:
      Tienda( int = 100, int = 10 );	// how many items, cart capacity
      Tienda( int, int, const int *, const int * );	// how many items, cart capacity, items values, items weights
      ~Tienda();
      std::string toString();
      void llenarCarrito();	// Maximize cart cost without exceed capacity (to be completed by students)
      void llenarCarritoPthreads( int family ); // same as above, recieves an int number of threads( represented by a family )

   private:
      int articulos;		// store items
      int * pesos;		// items weights
      int * precios;		// items costs
      int capacidadCarrito;	// cart capacity

};