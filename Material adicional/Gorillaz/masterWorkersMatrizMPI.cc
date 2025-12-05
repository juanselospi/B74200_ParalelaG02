/**
 * ============================================================
 *              EJERCICIO MASTER – WORKER (MPI)
 * ============================================================
 *
 * ENUNCIADO (típico de examen):
 *
 * Se tiene una TABLA (matriz) de tamaño N×N llena de valores enteros.
 * El proceso 0 (MASTER) debe:
 *      - Crear la tabla (o leerla)
 *      - Dividir las filas equitativamente entre los WORKERS
 *      - Enviar a cada worker su bloque de filas
 *      - Recibir la suma parcial calculada por cada worker
 *      - Calcular la suma TOTAL de toda la tabla
 *
 * Los procesos WORKER (ranks 1..p-1) deben:
 *      - Recibir su rango de filas
 *      - Sumar todos los valores en esas filas
 *      - Devolver la suma parcial al master
 *
 * OBJETIVO EN MPI:
 *      - Enviar bloques manualmente (no usar scatter)
 *      - Trabajar con MPI_Send / MPI_Recv
 *      - Entender la división de trabajo tipo MASTER–WORKER
 * ============================================================
 */

#include <mpi.h>
#include <cstdio>
#include <cstdlib>

#define N 8   // tamaño de la tabla N×N (puedes aumentar si quieres tamaño grande)

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // quién soy
    MPI_Comm_size(MPI_COMM_WORLD, &size);   // cuántos somos

    int matriz[N][N];  // tabla grande simulada (8×8 para ejemplo)

    // ================================================
    // PROCESO MASTER (rank 0)
    // ================================================
    if (rank == 0)
    {
        printf("=== MASTER – WORKER: Suma de tabla %dx%d ===\n", N, N);

        // ---------------------------
        // 1. Inicializar la matriz
        // ---------------------------
        int valor = 1;
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                matriz[i][j] = valor++;   // números 1..64

        printf("Matriz generada por el MASTER:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++)
                printf("%3d ", matriz[i][j]);
            printf("\n");
        }

        // ---------------------------
        // 2. Dividir filas entre workers
        // ---------------------------
        int workers = size - 1;
        int filasPorWorker = N / workers;  // división simple

        int filaInicio = 0;

        // ---------------------------
        // 3. ENVIAR TRABAJO A CADA WORKER
        // ---------------------------
        for (int w = 1; w < size; w++)
        {
            int inicio = filaInicio;
            int fin = filaInicio + filasPorWorker - 1;

            // Enviar rango de filas
            MPI_Send(&inicio, 1, MPI_INT, w, 0, MPI_COMM_WORLD);
            MPI_Send(&fin,    1, MPI_INT, w, 0, MPI_COMM_WORLD);

            // Enviar bloque de filas completas
            MPI_Send(&matriz[inicio],     // dirección de la primera fila
                     filasPorWorker * N,  // cantidad total de enteros
                     MPI_INT,
                     w, 1, MPI_COMM_WORLD);

            filaInicio += filasPorWorker;
        }

        // ---------------------------
        // 4. RECIBIR SUMAS PARCIALES
        // ---------------------------
        long sumaTotal = 0;
        for (int w = 1; w < size; w++)
        {
            long parcial;
            MPI_Recv(&parcial, 1, MPI_LONG, w, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sumaTotal += parcial;
        }

        printf("\nSUMA TOTAL DE LA MATRIZ = %ld\n", sumaTotal);
    }

    // ================================================
    // PROCESOS WORKER (rank >= 1)
    // ================================================
    else
    {
        int inicio, fin;

        // ---------------------------
        // 1. Recibir rango de filas
        // ---------------------------
        MPI_Recv(&inicio, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&fin,    1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int filas = fin - inicio + 1;

        // ---------------------------
        // 2. Recibir las filas reales
        // ---------------------------
        int* bloque = (int*) malloc(filas * N * sizeof(int));

        MPI_Recv(bloque, filas * N, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // ---------------------------
        // 3. Calcular suma local
        // ---------------------------
        long sumaLocal = 0;
        for (int i = 0; i < filas; i++)
            for (int j = 0; j < N; j++)
                sumaLocal += bloque[i * N + j];

        free(bloque);

        // ---------------------------
        // 4. Enviar resultado al master
        // ---------------------------
        MPI_Send(&sumaLocal, 1, MPI_LONG, 0, 2, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
