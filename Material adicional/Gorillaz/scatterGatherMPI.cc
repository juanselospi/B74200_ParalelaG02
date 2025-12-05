/*
El proceso 0 genera un arreglo de N enteros.  
Se desea dividir este arreglo equitativamente entre todos los procesos 
usando MPI_Scatter.  

Cada proceso debe:
  - recibir su bloque local,
  - calcular la suma local de esos elementos.

Finalmente, el proceso 0 debe:
  - reunir la suma total usando MPI_Reduce,
  - imprimir el resultado final.

Implemente la solución usando MPI con las funciones:
  MPI_Scatter, MPI_Reduce, MPI_Comm_rank, MPI_Comm_size.
*/

/**
 * Ejercicio: Suma total usando Scatter + Reduce
 *
 * El proceso 0 crea un arreglo de N números.
 * MPI reparte partes iguales entre los procesos con MPI_Scatter.
 * Cada proceso calcula su suma local.
 * Finalmente MPI_Reduce suma todas las sumas parciales y el proceso 0 imprime el total.
 */

#include <mpi.h>
#include <cstdio>
#include <cstdlib>

#define N 100000    // tamaño total del arreglo

int main(int argc, char **argv) {

    // ------------------------------------------------------------
    // Inicializar MPI
    // ------------------------------------------------------------
    MPI_Init(&argc, &argv);

    int id;       // rank del proceso
    int nproc;    // número total de procesos

    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    // ------------------------------------------------------------
    // Cada proceso recibirá N / nproc elementos
    // (se asume que N es divisible entre el número de procesos)
    // ------------------------------------------------------------
    int tamLocal = N / nproc;

    // ------------------------------------------------------------
    // Arreglo global (solo lo tiene el proceso 0)
    // ------------------------------------------------------------
    int *A = NULL;
    if (id == 0) {
        A = (int*) malloc(N * sizeof(int));

        // Llenar el arreglo con valores simples para probar (todos 1)
        for (int i = 0; i < N; i++)
            A[i] = 1;
    }

    // ------------------------------------------------------------
    // Arreglo local para cada proceso
    // Aquí MPI_Scatter depositará mi parte del arreglo
    // ------------------------------------------------------------
    int *local = (int*) malloc(tamLocal * sizeof(int));

    // ------------------------------------------------------------
    // MPI_Scatter:
    //  - Proceso 0 envía A dividido en bloques iguales
    //  - Cada proceso recibe tamLocal elementos en "local"
    // ------------------------------------------------------------
    MPI_Scatter(
        A,                // datos de entrada (solo en proceso 0)
        tamLocal,         // cuántos elementos envía a cada proceso
        MPI_INT,          // tipo de dato de los elementos enviados
        local,            // buffer donde recibo mis datos
        tamLocal,         // cuántos elementos recibo
        MPI_INT,          // tipo de dato recibido
        0,                // root (proceso que reparte)
        MPI_COMM_WORLD    // comunicador global
    );

    // ------------------------------------------------------------
    // Calcular suma LOCAL
    // ------------------------------------------------------------
    long sumaLocal = 0;
    for (int i = 0; i < tamLocal; i++)
        sumaLocal += local[i];

    // ------------------------------------------------------------
    // MPI_Reduce:
    //  - Cada proceso aporta su sumaLocal
    //  - Se aplica operación MPI_SUM
    //  - El proceso 0 recibe el resultado final en sumaGlobal
    // ------------------------------------------------------------
    long sumaGlobal = 0;

    MPI_Reduce(
        &sumaLocal,       // valor local
        &sumaGlobal,      // resultado final (solo en proceso 0)
        1,                // cantidad de elementos a reducir
        MPI_LONG,         // tipo de dato
        MPI_SUM,          // operación de reducción
        0,                // root (quien recibe el resultado)
        MPI_COMM_WORLD
    );

    // ------------------------------------------------------------
    // Solo el proceso 0 imprime el resultado
    // ------------------------------------------------------------
    if (id == 0)
        printf("Suma total = %ld\n", sumaGlobal);

    // ------------------------------------------------------------
    // Liberar memoria
    // ------------------------------------------------------------
    free(local);
    if (id == 0) free(A);

    // ------------------------------------------------------------
    // Finalizar MPI
    // ------------------------------------------------------------
    MPI_Finalize();
    return 0;
}

