/**
 *   Representa un punto en dos dimensiones
 *
**/


#include <cstdio>
#include "Punto.h"

/**
 *   Constructor básico, deja las coordenadas en cero
**/
Punto::Punto() {

   this->x = 0;
   this->y = 0;
   this->y = 0;

}

/**
 *   Constructor, inicializa el punto con los parámetros indicados
**/
Punto::Punto( double x, double y, double z ) {

   this->x = x;
   this->y = y;
   this->z = z;

}


/**
 *  Devuelve el valor de la coordenada x
**/
double Punto::demeX() {

   return this->x;

}


/**
 *  Retorna el valor de la coordenada y
**/
double Punto::demeY() {

   return this->y;

}


/**
 *  Retorna el valor de la coordenada z
**/
double Punto::demeZ() {
   return this->z;
}


/**
 *  Cambia el valor de las coordenadas
**/
void Punto::ponga( double vx, double vy, double vz ) {

   this->x = vx;
   this->y = vy;
   this->z = vz;

}


/**
 *   Calcula cuadrado de la distancia entre dos puntos (Euclidiana)
 *   Por el momento utiliza solo dos coordenadas x y y
 *
**/
// double Punto::dist2( Punto * otro ) {
//    double dx = this->x - otro->x;
//    double dy = this->y - otro->y;
//    double resultado;

//    resultado = dx * dx + dy * dy;

//    return resultado;

// }


double Punto::dist2( Punto * otro ) {
   double dx = this->x - otro->x;
   double dy = this->y - otro->y;
   double dz = this->z - otro->z;
   double resultado;

   resultado = dx * dx + dy * dy + dz * dz;

   return resultado;
}



/**
 *  Suma desplazamientos a un punto
**/
void Punto::sume( Punto * sumando ) {

   this->x += sumando->x;
   this->y += sumando->y;
   this->z += sumando->z;

}


/**
 *  Divide las coordenas de un punto por el mismo valor
**/
void Punto::divida( double div ) {

   this->x /= div;
   this->y /= div;
   this->z /= div;

}


/**
 *  Despliega la información de un punto
**/
std::string Punto::ver() {
   std::string disp;

   disp = "[ " + std::to_string( this->x ) + ", " + std::to_string( this->y ) + ", " + std::to_string( this->z ) + " ]";

   return disp;

}


