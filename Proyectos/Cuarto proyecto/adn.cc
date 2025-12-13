/**
 * Represent a DNA sequence
 *
 * @author	Programacion Concurrente
 * @version	2025/Nov/03
 * Grupos 2 y 3
 *
**/

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <unistd.h>
#include <ctime>

#include "adn.h"

/**
 * Default constructor
 * Create a random length sequence and fill with (allowed) components
 *
**/
ADN::ADN( int length ) {
   int posicion;
   char base = 'A';

   // srand( getpid() );

   srand( time(NULL) + clock() );

   rand();
 
   this->sequence = std::string( "" );
   for ( posicion = 0; posicion < length; posicion++ ) {
      switch ( rand() % 4 ) {
         case 0:
            base = 'G';	// Nucleotids
            break;
         case 1:
            base = 'T';
            break;
         case 2:
            base = 'C';
            break;
         case 3:
            base = 'A';
            break;
      }
      this->sequence.push_back( base );
   }

}


/**
 * Parameter constructor
 * @param	char	an array of chars with DNA components
 *
**/
ADN::ADN( const char * string ) {
   char base;
   int posicion;
   int length = strlen( string ) + 1;

   this->sequence = std::string( "" );
   for (int posicion = 0; posicion < strlen( string ); posicion++ ) {
      switch ( string[ posicion ] ) {	// The sequence can only have A, C, G, T components, any other letter is changed to A
         case 'A':
         case 'C':
         case 'G':
         case 'T':
            this->sequence.push_back( string[ posicion ] );
            break;
         default:
            this->sequence.push_back( 'A' );
            break;
      }
   }
}


/**
 *
 *
**/
ADN::~ADN() {
}


/**
 * Return a String with sequence
**/
std::string ADN::toString() {
   return this->sequence;
}



/**
 *
**/
void ADN::printSeqs() {
   printSeqs( this->sequence, -1, this->sequence.size() );
}


/**
 *
**/
void ADN::printSeqs( std::string str, int start, int end, std::string curStr ) {
	//base case
	if (start == end) {
		return;
	}
	//print current string permutation
	printf( "%s\n", curStr.c_str() );
	for ( int i = start + 1; i< end; i++ ) {
		curStr += str[i];
		printSeqs( str, i, end, curStr );
		curStr = curStr.erase( curStr.size() - 1 );
	}
}

