/**
 * EJERCICIO MPI – Usar MPI_Reduce para sumar un arreglo grande
 *
 * Enunciado típico de examen:
 *
 * Suponga que tenemos un "arreglo global" de N elementos,
 * repartido equitativamente entre P procesos.
 *
 * Cada proceso:
 *   - Tiene un subarreglo local con N/P elementos (su parte del arreglo global).
 *   - Calcula la suma LOCAL de sus elementos.
 *
 * Usando MPI_Reduce:
 *   - Todos los procesos envían su suma local.
 *   - El proceso 0 obtiene la suma TOTAL del arreglo completo.
 *
 * Implemente un programa que:
 *   1) Asuma que N es divisible entre el número de procesos.
 *   2) Cada proceso llene su subarreglo local con valores sencillos.
 *   3) Use MPI_Reduce para obtener la suma total en el proceso 0.
 *
 * Nota:
 *   En este ejemplo, no repartimos explícitamente el arreglo desde 0,
 *   sino que cada proceso "simula" tener su parte del arreglo global.
 *   Esto es muy común en ejercicios de libro y de examen.
 */

#include <mpi.h>
#include <cstdio>
#include <cstdlib>

#define N_TOTAL 100  // Tamaño "global" del arreglo (conceptual)

int main(int argc, char **argv) {

    // --------------------------------------------------------
    // Inicializamos MPI
    // --------------------------------------------------------
    MPI_Init(&argc, &argv);

    int id;      // rank del proceso actual
    int nproc;   // número total de procesos

    MPI_Comm_rank(MPI_COMM_WORLD, &id);     // obtengo mi ID (0,1,2,...)
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);  // obtengo cuántos procesos hay

    // --------------------------------------------------------
    // Verificar que N_TOTAL sea divisible entre nproc
    // para que todos tengan el mismo número de elementos.
    // --------------------------------------------------------
    if (N_TOTAL % nproc != 0) {
        if (id == 0) {
            printf("ERROR: N_TOTAL = %d no es divisible entre nproc = %d\n",
                   N_TOTAL, nproc);
        }
        MPI_Finalize();
        return 0;
    }

    // --------------------------------------------------------
    // Cada proceso tendrá LOCAL_N elementos de su "subarreglo"
    // --------------------------------------------------------
    int LOCAL_N = N_TOTAL / nproc;

    // Reservamos memoria para el subarreglo local
    int *local = (int*) malloc(LOCAL_N * sizeof(int));

    // --------------------------------------------------------
    // Inicializamos el subarreglo local.
    //
    // Podemos imaginar que el arreglo global es:
    //   G = [1, 1, 1, 1, ..., 1] con N_TOTAL unos.
    //
    // Entonces para simplificar:
    //   - Cada proceso llena su parte con 1's.
    //   - La suma total debería ser N_TOTAL.
    //
    // En un ejercicio más formal, podríamos usar:
    //   globalIndex = id * LOCAL_N + i;
    //   local[i] = globalIndex + 1;
    // y calcular la suma esperada a mano.
    // --------------------------------------------------------
    for (int i = 0; i < LOCAL_N; i++) {
        local[i] = 1;     // valor sencillo para verificar
    }

    // --------------------------------------------------------
    // Calculamos la suma LOCAL en cada proceso
    // --------------------------------------------------------
    int sumaLocal = 0;
    for (int i = 0; i < LOCAL_N; i++) {
        sumaLocal += local[i];
    }

    printf("Proceso %d: sumaLocal = %d\n", id, sumaLocal);

    // --------------------------------------------------------
    // Usamos MPI_Reduce para sumar todas las sumas locales.
    //
    // Parámetros:
    //   sendbuf  = &sumaLocal  (lo que envía cada proceso)
    //   recvbuf  = &sumaGlobal (dónde se guarda el resultado en el root)
    //   count    = 1           (un solo entero por proceso)
    //   datatype = MPI_INT
    //   op       = MPI_SUM     (suma)
    //   root     = 0           (proceso que recibe el resultado)
    // --------------------------------------------------------
    int sumaGlobal = 0;

    MPI_Reduce(
        &sumaLocal,        // buffer de envío (un entero)
        &sumaGlobal,       // buffer de recepción (solo root lo usa)
        1,                 // número de elementos a reducir
        MPI_INT,           // tipo de dato
        MPI_SUM,           // operación de reducción (suma)
        0,                 // root: quien va a recibir el resultado final
        MPI_COMM_WORLD
    );

    // --------------------------------------------------------
    // El proceso 0 imprime la suma global
    // --------------------------------------------------------
    if (id == 0) {
        printf("\nSuma GLOBAL de todo el arreglo = %d\n", sumaGlobal);
        printf("Valor esperado (N_TOTAL)        = %d\n", N_TOTAL);
    }

    // Liberamos memoria
    free(local);

    // Finalizamos MPI
    MPI_Finalize();
    return 0;
}
