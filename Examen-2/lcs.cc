#include "lcs.h"

LCS::LCS() {

}


LCS::~LCS() {

}


string LCS::serialLCS( string& S1 ) {
    string masLargo = "";

    int initIndex = 0;
    int finIndex = 0;


    for(initIndex; initIndex < S1.size(); initIndex++) {

        string resultado = "";
        string resto = "";

        for(int index = S1.size(); index > initIndex; index--) {

            if(S1[initIndex] == S1[index] && finIndex == 0) {
                finIndex = index;
                
            }

        }

        //cout << initIndex << " mi initIndex" << endl;

        //cout << finIndex << " mi finIndex" << endl;

        int middle = ((finIndex - initIndex) / 2);

        //cout << middle << " mi middle" << endl;


        // int prueba = 5 % 2;

        // cout << "PRUEBA " << prueba << endl;


        // si es par modalidad pieza central
        if( (finIndex % 2) == 0 ) {

            for( int initial = initIndex; initial <= middle; initial++ ) {

                if(S1[middle - initial] == S1[middle + initial]) {

                    resultado += S1[middle + initial];
                }
            }

            for(int i = resultado.size(); i > 0; i--) {
                resto = resto + resultado[i];
            }

            resto = resto + resultado;

            if(resto.size() > masLargo.size()) {
                masLargo = resto;
            }


        // si no es par verificar centros
        } else if ( (finIndex % 2) == 1 )  {

            //cout << "ENTRE!" << endl;

            // comparar medios
            if(S1[middle] == S1[middle + 1]) {

                //cout << "ENTRE!" << endl;

                resultado += S1[middle];
                resultado += S1[middle + 1];

                for(int initial = initIndex + 1; initial <= middle; initial++ ) {

                    if(S1[middle - initial] == S1[(middle + 1) + initial]) {

                        resultado += S1[(middle + 1) + initial];
                    }
                }

                for(int i = resultado.size(); i > 1; i--) {
                    resto = resto + resultado[i];
                }

                resto = resto + resultado;

                if(resto.size() > masLargo.size()) {
                    masLargo = resto;
                }

            }

        }

    }

    return masLargo;
}






string LCS::paralelaLCS( string& S1, int hilos ) {

    string masLargo = "";


    #pragma omp parallel num_threads(hilos)
    {

        int tid = omp_get_thread_num();

        // separo los hilos para que cada uno calcule dentro de cierto indice y evito condiciones de carrera y overhead por coordinacion de acceso
        int bloque = S1.size() / hilos;
        int inicio = tid * bloque;
        int fin = (tid == hilos - 1) ? S1.size() : inicio + bloque;

        int finIndex = 0;

        for(int initIndex = inicio; initIndex < fin; initIndex++) {

            string resultado = "";
            string resto = "";

            for(int index = S1.size(); index > initIndex; index--) {

                if(S1[initIndex] == S1[index] && finIndex == 0) {
                    finIndex = index;
                    
                }

            }

            //cout << initIndex << " mi initIndex" << endl;

            //cout << finIndex << " mi finIndex" << endl;

            int middle = ((finIndex - initIndex) / 2);

            //cout << middle << " mi middle" << endl;


            // int prueba = 5 % 2;

            // cout << "PRUEBA " << prueba << endl;


            // si es par modalidad pieza central
            if( (finIndex % 2) == 0 ) {

                for( int initial = initIndex; initial <= middle; initial++ ) {

                    if(S1[middle - initial] == S1[middle + initial]) {

                        resultado += S1[middle + initial];
                    }
                }

                for(int i = resultado.size(); i > 0; i--) {
                    resto = resto + resultado[i];
                }

                resto = resto + resultado;

                #pragma omp atomic
                if(resto.size() > masLargo.size()) {
                    masLargo = resto;
                }


            // si no es par verificar centros
            } else if ( (finIndex % 2) == 1 )  {

                //cout << "ENTRE!" << endl;

                // comparar medios
                if(S1[middle] == S1[middle + 1]) {

                    //cout << "ENTRE!" << endl;

                    resultado += S1[middle];
                    resultado += S1[middle + 1];

                    for(int initial = initIndex + 1; initial <= middle; initial++ ) {

                        if(S1[middle - initial] == S1[(middle + 1) + initial]) {

                            resultado += S1[(middle + 1) + initial];
                        }
                    }

                    for(int i = resultado.size(); i > 1; i--) {
                        resto = resto + resultado[i];
                    }

                    resto = resto + resultado;

                    #pragma omp atomic
                    if(resto.size() > masLargo.size()) {
                        masLargo = resto;
                    }

                }

            }

        }

    }

    return masLargo;
}
















// string LCS::paralelaLCS( string& S1, string& S2 ) {

//     int rank, size;

//     MPI_Comm_rank( MPI_COMM_WORLD, &rank );
//     MPI_Comm_size( MPI_COMM_WORLD, &size );

//     int n = S1.size();
//     int m = S2.size();

//     int filas    = n + 1;
//     int columnas = m + 1;

//     // matriz compartida pero cada proceso tendra su propia copia
//     matriz = vector< vector<int> >( filas, vector<int>( columnas, 0 ) );

//     // indice lineal dentro de la matriz
//     auto getCell = [&]( int i, int j ) -> int& {
//         return matriz[ i ][ j ];
//     };

//     int totalDiag = n + m;

//     // recorro diagonales k = i + j
//     for( int k = 2; k <= totalDiag; k++ ) {

//         // rango de i valido para esta diagonal
//         int i_min = max( 1, k - m );
//         int i_max = min( n, k - 1 );

//         int diagLen = i_max - i_min + 1; // cantidad de celdas en esta diagonal

//         if( diagLen <= 0 ) {
//             // si no hay nada que hacer en esta diagonal
//             MPI_Barrier( MPI_COMM_WORLD );
//             continue;
//         }

//         // vectores local y global con los valores de la diagonal actual
//         vector<int> diagLocal( diagLen, 0 );
//         vector<int> diagGlobal( diagLen, 0 );

//         // division simple de la diagonal entre procesos
//         int chunk = ( diagLen + size - 1 ) / size;

//         int beginIndex = rank * chunk;
//         int endIndex   = min( diagLen, beginIndex + chunk ) - 1;

//         if( beginIndex < diagLen ) {
//             // si este proceso tiene al menos una celda que calcular
//             for( int idx = beginIndex; idx <= endIndex; idx++ ) {

//                 int i = i_min + idx;
//                 int j = k - i;

//                 if( j <= 0 || j > m ) {
//                     continue;
//                 }

//                 int arriba   = getCell( i - 1, j );
//                 int izquierda = getCell( i, j - 1 );
//                 int diag      = getCell( i - 1, j - 1 );

//                 if( S1[ i - 1 ] == S2[ j - 1 ] ) {
//                     diagLocal[ idx ] = diag + 1;
//                 } else {
//                     diagLocal[ idx ] = ( arriba > izquierda ? arriba : izquierda );
//                 }
//             }
//         }

//         // combina los datos de todos los procesos en la diagonal actual
//         MPI_Allreduce( diagLocal.data(), diagGlobal.data(), diagLen, MPI_INT, MPI_MAX, MPI_COMM_WORLD );

//         // escribe la diagonal completa en la matriz local de cada proceso
//         for( int idx = 0; idx < diagLen; idx++ ) {
//             int i = i_min + idx;
//             int j = k - i;

//             if( j > 0 && j <= m ) {
//                 getCell( i, j ) = diagGlobal[ idx ];
//             }
//         }

//         MPI_Barrier( MPI_COMM_WORLD );
//     }

//     // reconstruccion de la LCS resultado por el rank 0
//     string resultado = "";

//     if( rank == 0 ) {
//         int i = n;
//         int j = m;

//         while( i > 0 && j > 0 ) {

//             if( S1[ i - 1 ] == S2[ j - 1 ] ) {
//                 // este caracter forma parte de la subsecuencia
//                 resultado = S1[ i - 1 ] + resultado;
//                 i--;
//                 j--;
//             } else {
//                 int arriba   = getCell( i - 1, j );
//                 int izquierda = getCell( i, j - 1 );

//                 if( arriba >= izquierda ) {
//                     i--;
//                 } else {
//                     j--;
//                 }
//             }
//         }
//     }

//     return resultado;
// }
