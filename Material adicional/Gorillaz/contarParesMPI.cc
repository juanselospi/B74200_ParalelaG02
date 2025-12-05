/*
   Ejercicio MPI — Procesar un arreglo grande y contar algo

   El proceso 0 genera un arreglo grande de números enteros.
   Luego distribuye partes iguales del arreglo a todos los procesos
   usando MPI_Scatter.

   Cada proceso cuenta localmente cuántos números cumplen cierto criterio:
       - En este ejemplo: cuántos números SON PARES.

   Finalmente, se usa MPI_Reduce para sumar todas las cuentas locales
   en el proceso 0, el cual imprime el resultado final.

   Este patrón es extremadamente común en exámenes:
     - Scatter para distribuir trabajo
     - Cómputo local por proceso
     - Reduce para combinar resultados
*/

#include <mpi.h>
#include <cstdio>
#include <cstdlib>

#define N 1000000   // tamaño del arreglo total

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // id del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &size);   // número total de procesos

    int *bigArray = nullptr;   // solo lo usa el proceso 0
    int chunk = N / size;      // cantidad de datos por proceso

    // -------------------------
    // PROCESO 0 GENERA EL ARREGLO
    // -------------------------
    if (rank == 0) {
        bigArray = (int*) malloc(N * sizeof(int));
        for (int i = 0; i < N; i++)
            bigArray[i] = rand() % 1000;   // números de 0 a 999
    }

    // -------------------------
    // Cada proceso recibe chunk elementos
    // -------------------------
    int* localData = (int*) malloc(chunk * sizeof(int));

    MPI_Scatter(
        bigArray,        // datos a repartir (solo válido en rank 0)
        chunk,           // cantidad por proceso
        MPI_INT,         // tipo de dato
        localData,       // buffer local donde recibo mis datos
        chunk,           
        MPI_INT,
        0,               // root
        MPI_COMM_WORLD
    );

    // -------------------------
    // CADA PROCESO CUENTA SUS PARES
    // -------------------------
    int localCount = 0;

    for (int i = 0; i < chunk; i++) {
        if (localData[i] % 2 == 0)
            localCount++;
    }

    // -------------------------
    // REDUCE PARA SUMAR TODOS LOS RESULTADOS
    // -------------------------
    int totalCount = 0;

    MPI_Reduce(
        &localCount,    // valor local
        &totalCount,    // resultado final (solo en root)
        1,              // un número entero
        MPI_INT,
        MPI_SUM,        // operación: sumar
        0,              // root
        MPI_COMM_WORLD
    );

    // -------------------------
    // PROCESO 0 IMPRIME EL RESULTADO FINAL
    // -------------------------
    if (rank == 0) {
        printf("Total de números pares en el arreglo = %d\n", totalCount);
        free(bigArray);
    }

    free(localData);

    MPI_Finalize();
    return 0;
}
