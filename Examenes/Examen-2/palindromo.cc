#include <iostream>
#include <stdio.h>
#include <string>
#include <omp.h>

#include "adn.h"
#include "lcs.h"

#define INPUT 50
#define HILOS 4

using namespace std;

void printSubsequences( std::string str, int start, int end, std::string curStr = "" ) {
	//base case
	if (start == end) {
		return;
	}
	//print current string permutation
	printf( "%s\n", curStr.c_str() );
	for (int i = start + 1; i< end; i++) {
		curStr += str[i];
		printSubsequences(str, i, end, curStr);
		curStr = curStr.erase( curStr.size() - 1 );
	}
}



int main( int argumentos, char ** valores ) {
	double start, finish, wusedSerial, wusedParallel;
	string S1;


	int misHilos = HILOS;

	int input1 = INPUT;

	// parametros ingresados por el usuario
    if( argumentos > 1 ) {

        input1 = atoi( valores[ 1 ] );

		// si me dan un valor invalido irme al default
		if(input1 < 1) {

			input1 = INPUT;
		}
    }

    if( argumentos > 2 ) {

        misHilos = atoi(valores[2]);

		if( misHilos < 1 ) {

			misHilos = HILOS;
		}
    }


	//ADN * adn1 = new ADN( "CCAACC" );

	ADN * adn1 = new ADN( input1 );

	S1 = adn1->toString();


	cout << " Cadena original: " << S1 << endl;

	LCS solucionador;

	start = omp_get_wtime();
	string serial = solucionador.serialLCS( S1 );
	finish = omp_get_wtime();
	wusedSerial = finish - start;

	cout << "palindromo serial para cadenas de tamaño: " << input1 << " encontrada:" << endl;
	cout << serial << endl;
	cout << "Longitud del palindromo serial: " << serial.size() << endl;

	cout << "Tiempo version serial: " << wusedSerial << " segundos" << endl;


	// VERSION PARALELA AQUI

	LCS paralelo;

	start = omp_get_wtime();
	string paralela = paralelo.paralelaLCS( S1, misHilos );
	finish = omp_get_wtime();
	wusedParallel = finish - start;

	cout << "palindromo paralelo para cadenas de tamaño: " << input1 << " encontrada:" << endl;
	cout << paralela << endl;
	cout << "Longitud del palindromo paralelo: " << paralela.size() << endl;

	cout << "Tiempo version paralela: " << wusedParallel << " segundos" << endl;




	delete adn1;
}


/*

prueba de que los palindromos compilan y corren normal, pero no se si hay algo con la configuracion del .vscode o el path que no sirve

 I  ~/Desktop/VSCode/Material adicional/Examen-2  make                                                                         ✔  16:34:37 

g++    -c -o palindromo.o palindromo.cc
g++    -c -o adn.o adn.cc
g++    -c -o lcs.o lcs.cc
g++ -g -fopenmp palindromo.o adn.o lcs.o -o palindromo.out
mpic++ -g anilloFibonacci.cc -o anilloFibonacci.out

 I  ~/Desktop/VSCode/Material adicional/Examen-2  ./palindromo.out                                                             ✔  16:40:02 

 Cadena original: TCGGAATGAGTGATCATTGGCACAAACAGTATAATCACCCAGCCTTTCCC
palindromo serial para cadenas de tamaño: 50 encontrada:
TCGAATATATATAAGCT
Longitud del palindromo serial: 18
Tiempo version serial: 4.6613e-05 segundos
palindromo paralelo para cadenas de tamaño: 50 encontrada:
TCGAATATATATAAGCT
Longitud del palindromo paralelo: 18
Tiempo version paralela: 2.6363e-05 segundos

 I  ~/Desktop/VSCode/Material adicional/Examen-2     

*/