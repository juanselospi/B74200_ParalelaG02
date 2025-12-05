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




















// int main( int argumentos, char ** valores ) {
// 	double start, finish, wusedSerial, wusedParallel;
// 	string S1, S2;

// 	int input1 = INPUT;
// 	int input2 = INPUT;

// 	// parametros ingresados por el usuario
//     if( argumentos > 1 ) {

//         input1 = atoi( valores[ 1 ] );

// 		// si me dan un valor invalido irme al default
// 		if(input1 < 1) {

// 			input1 = INPUT;
// 		}
//     }

//     if( argumentos > 2 ) {

//         input2 = atoi(valores[2]);

// 		if( input2 < 1 ) {

// 			input2 = INPUT;
// 		}
//     }

// 	// inicializacion de variables por mejorar
// 	MPI_Init( &argumentos, &valores );

// 	int rank, size;

// 	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
// 	MPI_Comm_size( MPI_COMM_WORLD, &size );

// 	// solo el hilo de ranking 0 crea las cadenas y corre serialmente
// 	if( rank == 0 ) {
// 		// ADN * adn1 = new ADN( "ACCGGTCGAGTGCGCGGAAGCCGGCCGAA" );
// 		// ADN * adn2 = new ADN( "GTCGTTCGGAATGGCCGTTGCTCTGTAA" );
// 		// ADN * adn3 = new ADN( 1024 );
// 		// std::string test = "ACCGGT";

// 		//   printf( "Random sequence: %s\n", adn3->toString().c_str() );
// 		//   adn1->printSeqs();

// 		ADN * adn1 = new ADN( input1 );
// 		ADN * adn2 = new ADN( input2 );

// 		S1 = adn1->toString();
// 		S2 = adn2->toString();

// 		// para revisar que ambas cadenas aleatorias sean diferentes
// 		// cout << "\nS1: " << S1 << endl;
// 		// cout << "\nS2: " << S2 << endl;


// 		// CORRIDA SERIAL
// 		LCS solucionador;

// 		start = MPI_Wtime();
// 		string serial = solucionador.serialLCS( S1, S2 );
// 		finish = MPI_Wtime();
// 		wusedSerial = finish - start;

// 		cout << "LCS serial para cadenas de tamaños: " << input1 << " y " << input2 << " encontrada:" << endl;
// 		cout << serial << endl;
// 		cout << "Longitud de la LCS serial: " << serial.size() << endl;

// 		cout << "Tiempo version serial: " << wusedSerial << " segundos" << endl;

// 		// delete adn3;
// 		delete adn2;
// 		delete adn1;
// 	}

// 	int n, m;

// 	if( rank == 0 ) {
// 		n = S1.size();
// 		m = S2.size();
// 	}

// 	MPI_Bcast( &n, 1, MPI_INT, 0, MPI_COMM_WORLD );
//     MPI_Bcast( &m, 1, MPI_INT, 0, MPI_COMM_WORLD );

//     if( rank != 0 ){
//         S1.resize( n );
//         S2.resize( m );
//     }

// 	MPI_Bcast( &S1[ 0 ], n, MPI_CHAR, 0, MPI_COMM_WORLD );
// 	MPI_Bcast( &S2[ 0 ], m, MPI_CHAR, 0, MPI_COMM_WORLD );

// 	// VERSION PARALELA
// 	LCS wavefront;

// 	MPI_Barrier( MPI_COMM_WORLD );

// 	start = MPI_Wtime();
// 	string paralela = wavefront.paralelaLCS( S1, S2 );
// 	finish = MPI_Wtime();
// 	wusedParallel = finish - start;

// 	if( rank == 0 ) {

// 		cout << "\nLCS paralela por MPI para cadenas de tamaños: " << input1 << " y " << input2 << " encontrada:" << endl;
// 		cout << paralela << endl;
// 		cout << "Longitud de la LCS paralela: " << paralela.size() << endl;


// 		cout << "Tiempo version paralela: " << wusedParallel << " segundos" << endl;

// 		double speedUp = wusedSerial / wusedParallel;

// 		cout << "\nSpeedUp: " << speedUp << "x" << endl;
// 	}

// 	MPI_Finalize();
// }
