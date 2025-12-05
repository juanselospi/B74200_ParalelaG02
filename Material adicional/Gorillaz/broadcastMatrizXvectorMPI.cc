/**
 * EJERCICIO MPI – Broadcast + Matriz x Vector
 *
 * Enunciado típico de examen:
 *
 * El proceso 0 tiene:
 *   - una matriz A de tamaño N x N
 *   - un vector x de tamaño N
 *
 * Se desea calcular el producto y = A * x de forma paralela usando MPI.
 *
 * Requisitos:
 *   1) Las filas de la matriz A deben dividirse equitativamente
 *      entre todos los procesos usando MPI_Scatter.
 *
 *   2) El vector x debe ser enviado a TODOS los procesos usando MPI_Bcast.
 *
 *   3) Cada proceso calcula su parte local de y:
 *        y_local = A_local * x
 *      (son las filas que le tocaron a ese proceso).
 *
 *   4) El proceso 0 reúne las partes de y usando MPI_Gather
 *      y muestra el resultado final.
 *
 * Suponga que:
 *   - N es divisible entre el número de procesos.
 *   - La matriz se almacena como arreglo plano en memoria (row-major).
 *
 * Implemente esta solución usando:
 *   - MPI_Scatter
 *   - MPI_Bcast
 *   - MPI_Gather
 *   - MPI_Comm_rank, MPI_Comm_size
 */

#include <mpi.h>
#include <cstdio>
#include <cstdlib>

#define N 4   // Tamaño de la matriz (N x N) y del vector

int main(int argc, char **argv) {

    // --------------------------------------------------
    // Inicializar MPI
    // --------------------------------------------------
    MPI_Init(&argc, &argv);

    int id;      // rank (ID) del proceso actual
    int nproc;   // número total de procesos

    MPI_Comm_rank(MPI_COMM_WORLD, &id);   // obtengo mi rank
    MPI_Comm_size(MPI_COMM_WORLD, &nproc); // obtengo cantidad de procesos

    // Verificamos que N sea divisible entre nproc para simplificar
    if (N % nproc != 0) {
        if (id == 0)
            printf("ERROR: N (%d) no es divisible entre nproc (%d)\n", N, nproc);
        MPI_Finalize();
        return 0;
    }

    // --------------------------------------------------
    // Cada proceso procesará 'filasLocal' filas de la matriz
    // --------------------------------------------------
    int filasLocal = N / nproc;

    // --------------------------------------------------
    // Declaración de estructuras:
    //   - A: matriz completa (solo en el proceso 0)
    //   - x: vector completo (lo tendrá todos tras el broadcast)
    //   - Alocal: bloque de filas que le toca a este proceso
    //   - ylocal: parte local del resultado y (una fila por fila local)
    // --------------------------------------------------
    int *A = nullptr;        // solo la usa el proceso 0
    int x[N];                // vector entero de tamaño N
    int *Alocal = (int*) malloc(filasLocal * N * sizeof(int));
    int ylocal[filasLocal];  // resultado parcial

    // --------------------------------------------------
    // Inicialización de datos SOLO en proceso 0
    // --------------------------------------------------
    if (id == 0) {

        // Reservamos memoria para la matriz A (N x N)
        A = (int*) malloc(N * N * sizeof(int));

        // Llenamos la matriz A con valores sencillos para prueba
        // Por ejemplo:
        // A = [  1  2  3  4
        //        5  6  7  8
        //        9 10 11 12
        //       13 14 15 16 ]
        int valor = 1;
        for (int i = 0; i < N * N; i++) {
            A[i] = valor++;
        }

        // Llenamos el vector x con unos (1,1,1,1) para que sea fácil verificar
        for (int i = 0; i < N; i++) {
            x[i] = 1;
        }

        // Mostramos A y x (solo como referencia)
        printf("Matriz A:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++)
                printf("%3d ", A[i * N + j]);
            printf("\n");
        }

        printf("Vector x:\n");
        for (int i = 0; i < N; i++)
            printf("%3d ", x[i]);
        printf("\n\n");
    }

    // --------------------------------------------------
    // MPI_Scatter:
    //   - Divide la matriz A por bloques de filas.
    //   - Cada proceso recibe 'filasLocal * N' enteros en Alocal.
    //
    // Layout:
    //   Proceso 0: A[0 .. filasLocal*N-1]
    //   Proceso 1: A[filasLocal*N .. 2*filasLocal*N-1]
    //   ...
    // --------------------------------------------------
    MPI_Scatter(
        A,                    // buffer de envío (solo root)
        filasLocal * N,       // número de enteros que van a cada proceso
        MPI_INT,              // tipo de dato de la matriz
        Alocal,               // buffer de recepción local
        filasLocal * N,       // cuántos enteros recibe este proceso
        MPI_INT,              // tipo de dato recibido
        0,                    // root (proceso 0 envía)
        MPI_COMM_WORLD
    );

    // --------------------------------------------------
    // MPI_Bcast:
    //   - Envía el vector x completo a TODOS los procesos.
    //   - Después de esta llamada, cada proceso tiene el mismo x[].
    // --------------------------------------------------
    MPI_Bcast(
        x,            // buffer a difundir
        N,            // cantidad de elementos
        MPI_INT,      // tipo de dato
        0,            // root que lo envía
        MPI_COMM_WORLD
    );

    // --------------------------------------------------
    // Cálculo local:
    //   Cada proceso tiene:
    //     - filasLocal filas de A (en Alocal)
    //     - todo el vector x
    //
    //   Calcula:
    //     ylocal[i] = suma_j ( Alocal[i,j] * x[j] )
    //   para i en [0, filasLocal)
    // --------------------------------------------------
    for (int i = 0; i < filasLocal; i++) {
        int suma = 0;

        // Fila i local corresponde a la fila (id * filasLocal + i) global
        for (int j = 0; j < N; j++) {
            int a_ij = Alocal[i * N + j];  // elemento de la fila local i, columna j
            suma += a_ij * x[j];
        }

        ylocal[i] = suma;
    }

    // --------------------------------------------------
    // MPI_Gather:
    //   - Reúne todos los ylocal de cada proceso en el arreglo y global
    //     que solo existe en el proceso 0.
    //
    //   y tiene tamaño N (una entrada por fila de la matriz A).
    // --------------------------------------------------
    int y[N];   // solo tiene sentido completo en el proceso 0

    MPI_Gather(
        ylocal,          // buffer local a enviar
        filasLocal,      // cuántos elementos envía cada proceso
        MPI_INT,         // tipo de dato
        y,               // buffer de recepción (solo root 0)
        filasLocal,      // cuántos recibe desde cada proceso
        MPI_INT,
        0,               // root
        MPI_COMM_WORLD
    );

    // --------------------------------------------------
    // Proceso 0 imprime el resultado y
    // --------------------------------------------------
    if (id == 0) {
        printf("Resultado y = A * x:\n");
        for (int i = 0; i < N; i++)
            printf("%3d ", y[i]);
        printf("\n");
    }

    // --------------------------------------------------
    // Liberar memoria
    // --------------------------------------------------
    if (id == 0 && A != nullptr)
        free(A);

    free(Alocal);

    // Finalizar MPI
    MPI_Finalize();
    return 0;
}
