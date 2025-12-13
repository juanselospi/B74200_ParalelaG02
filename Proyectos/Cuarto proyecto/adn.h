/**
 * Represent a DNA sequence
 *
 * @author	Programacion Concurrente
 * @version	2025/Nov/03
 * Grupos  2 y 3
 *
**/
#pragma once

#include <string>


class ADN {
   public:
      ADN( int = 100 );
      ADN( const char * );
      ~ADN();
      std::string toString();
      void printSeqs();
      void printSeqs( std::string, int, int, std::string = "" );

   private:
      std::string sequence;
};

