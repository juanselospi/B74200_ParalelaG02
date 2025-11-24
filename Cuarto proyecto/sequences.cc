/**
 * Represent a DNA sequence
 * With normal components A, C, G, and T
 *
 * @author	Programacion Concurrente
 * @version	2020/Oct/2
 *
 * Grupos 2 y 3
 *
**/

/**
 *  Compilar con:
 *      make
 * 
 *  Esto genera el ejecutable principal: sequences.out
 *
 *  Parametros de ejecucion:
 * 		mpirun -np [hilos] ./sequences.out
 *
 *  Donde:
 *      hilos       -> cantidad de hilos MPI a usar (int)
 * 
 *  Ejemplo de ejecucion usado para pruebas:
 * 		mpirun -np 8 ./sequences.out
 */

#include <iostream>
#include <stdio.h>
#include <string>
#include <mpi.h>

#include "adn.h"
#include "lcs.h"

using namespace std;

/**
 *
**/
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
	string S1, S2;

	// inicializacion de variables por mejorar
	MPI_Init( &argumentos, &valores );

	int rank, size;

	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );

	// solo el hilo de ranking 0 crea las cadenas y corre serialmente
	if( rank == 0 ) {
		// ADN * adn1 = new ADN( "ACCGGTCGAGTGCGCGGAAGCCGGCCGAA" );
		// ADN * adn2 = new ADN( "GTCGTTCGGAATGGCCGTTGCTCTGTAA" );
		// ADN * adn3 = new ADN( 1024 );
		// std::string test = "ACCGGT";

		//   printf( "Random sequence: %s\n", adn3->toString().c_str() );
		//   adn1->printSeqs();

		ADN * adn1 = new ADN( 10000 );
		ADN * adn2 = new ADN( 10000 );

		S1 = adn1->toString();
		S2 = adn2->toString();

		// CORRIDA SERIAL
		LCS solucionador;

		start = MPI_Wtime();
		string serial = solucionador.serialLCS( S1, S2 );
		finish = MPI_Wtime();
		wusedSerial = finish - start;

		cout << "LCS serial de tamaño [" << serial.size() << "] encontrada:" << endl;
		cout << serial << endl;
		cout << "\nTiempo version serial: " << wusedSerial << " segundos" << endl;

		// delete adn3;
		delete adn2;
		delete adn1;
	}

	int n, m;

	if( rank == 0 ) {
		n = S1.size();
		m = S2.size();
	}

	MPI_Bcast( &n, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &m, 1, MPI_INT, 0, MPI_COMM_WORLD );

    if( rank != 0 ){
        S1.resize( n );
        S2.resize( m );
    }

	MPI_Bcast( &S1[ 0 ], n, MPI_CHAR, 0, MPI_COMM_WORLD );
	MPI_Bcast( &S2[ 0 ], m, MPI_CHAR, 0, MPI_COMM_WORLD );

	// VERSION PARALELA
	LCS wavefront;

	MPI_Barrier( MPI_COMM_WORLD );

	start = MPI_Wtime();
	string paralela = wavefront.paralelaLCS( S1, S2 );
	finish = MPI_Wtime();
	wusedParallel = finish - start;

	if( rank == 0 ) {

		cout << "\nLCS paralela con " << size << " procesos y de tamaño [" << paralela.size() << "] encontrada:" << endl;
		cout << paralela << endl;
		cout << "\nTiempo version paralela: " << wusedParallel << " segundos" << endl;

		double speedUp = wusedSerial / wusedParallel;

		cout << "\nSpeedUp: " << speedUp << "x" << endl;
	}

	MPI_Finalize();
}
