/**
 * ================================================================
 *                 EJERCICIO: sumaUno con MPI
 * ================================================================
 * Enunciado:
 *    Se desea calcular el total resultante de sumar "1" muchas veces.
 *    Cada proceso realizará una cantidad fija de incrementos (por ejemplo 1000),
 *    acumulando su resultado en una variable local.
 *
 *    Al final, el proceso 0 (root) debe obtener la suma total combinando
 *    los resultados de todos los procesos usando MPI_Reduce.
 *
 *    Este ejercicio es equivalente a sumaUno con Pthreads/OpenMP,
 *    pero como MPI NO usa memoria compartida, aquí cada proceso acumula
 *    su propio subtotal y luego se combinan con una reducción.
 *
 * Idea clave:
 *      totalFinal = sumatoria( subtotalProceso[i] )  para i = 0..numProcesos-1
 *
 * ================================================================
 */

#include <mpi.h>
#include <cstdio>

#define SUMAS_POR_PROCESO 1000   // Cada proceso sumará 1 mil veces

int main(int argc, char** argv) {

    // ------------------------------------------------------------
    // Inicializar MPI
    // ------------------------------------------------------------
    MPI_Init(&argc, &argv);

    int rank, size;

    // rank = número de proceso   (0 = master)
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // size = cantidad total de procesos
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // ------------------------------------------------------------
    // Cada proceso calcula su suma local
    // ------------------------------------------------------------
    long subtotal = 0;

    // Cada proceso suma "1" un número fijo de veces
    for (int i = 0; i < SUMAS_POR_PROCESO; i++) {
        subtotal += 1;
    }

    // ------------------------------------------------------------
    // Ahora combinamos los resultados de TODOS los procesos.
    // Se usa MPI_Reduce:
    //
    //    MPI_Reduce(
    //          &variable_local,
    //          &variable_final (solo root),
    //          1,
    //          MPI_LONG,
    //          MPI_SUM,
    //          root (0),
    //          MPI_COMM_WORLD
    //    );
    //
    // root = 0 recibe la suma global en "totalFinal"
    // ------------------------------------------------------------
    long totalFinal = 0;

    MPI_Reduce(&subtotal,       // dato que envía cada proceso
               &totalFinal,     // donde root recibe el total
               1,               // enviamos 1 número
               MPI_LONG,        // tipo del dato
               MPI_SUM,         // operación: SUMA
               0,               // root es el proceso 0
               MPI_COMM_WORLD); // comunicador global

    // ------------------------------------------------------------
    // Solo el proceso 0 imprime el resultado final
    // ------------------------------------------------------------
    if (rank == 0) {
        printf("Total final = %ld (con %d procesos)\n",
               totalFinal, size);
    }

    // ------------------------------------------------------------
    // Finaliza MPI
    // ------------------------------------------------------------
    MPI_Finalize();
    return 0;
}
