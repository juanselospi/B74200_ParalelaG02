/***************************************************************
 *  PROBLEMA DE GOLDBACH — VERSIÓN MPI
 *
 *  Enunciado:
 *  ==========
 *  Se tiene una secuencia de números pares desde un valor inicial
 *  (por defecto 6) hasta un valor final (por defecto 100).
 *
 *  Según la conjetura de Goldbach, TODO número par N ≥ 6 puede
 *  escribirse como:
 *
 *        N = p + q     donde p y q son números primos.
 *
 *  Objetivo:
 *  ---------
 *  Implementar este cálculo usando MPI:
 *     - El proceso 0 divide el rango entre los procesos.
 *     - Cada proceso busca p y q para los pares asignados.
 *     - Cada proceso imprime sus soluciones.
 *
 *  Este ejercicio es típico de examen de MPI porque:
 *     ✓ Divide trabajo de forma estática
 *     ✓ Requiere comunicación mínima (solo broadcast de parámetros)
 *     ✓ Resultado no determinístico en orden → indica paralelismo real
 ****************************************************************/

#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>

// ------------------------------------------------------------
// Función para verificar si un número es primo
// ------------------------------------------------------------
bool esPrimo(int n) {
    if (n < 2) return false;
    if (n % 2 == 0) return (n == 2);
    for (int i = 3; i * i <= n; i += 2)
        if (n % i == 0)
            return false;
    return true;
}

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int id, nproc;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);     // ID del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);  // # de procesos total

    // ----------------------------------------
    // Parámetros del rango (pueden cambiarse)
    // ----------------------------------------
    int inicio = 6;
    int fin    = 100;

    if (id == 0)
        printf("=== GOLD BACH con MPI ===\nProcesando pares de %d a %d...\n\n",
               inicio, fin);

    // ----------------------------------------
    // BROADCAST: enviar parámetros a todos
    // ----------------------------------------
    MPI_Bcast(&inicio, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&fin,    1, MPI_INT, 0, MPI_COMM_WORLD);

    // ----------------------------------------
    // Calcular cuántos pares hay
    // ----------------------------------------
    int totalPares = ((fin - inicio) / 2) + 1;  // ejemplo: 6..100 → 48 pares

    // ----------------------------------------
    // Dividir trabajo por proceso
    // ----------------------------------------
    int base = totalPares / nproc;
    int extra = totalPares % nproc;

    // Cada proceso recibe su bloque:
    int miCantidad = base + (id < extra ? 1 : 0);

    // Calcular índice de inicio en la lista de pares
    int miInicioIndex;
    if (id < extra)
        miInicioIndex = id * (base + 1);
    else
        miInicioIndex = extra * (base + 1) + (id - extra) * base;

    // Convertir índice a número par real:
    int miInicioPar = inicio + 2 * miInicioIndex;

    // ----------------------------------------
    // BÚSQUEDA DE GOLD BACH EN PARALELO
    // ----------------------------------------
    for (int k = 0; k < miCantidad; k++) {

        int n = miInicioPar + 2 * k;  // número par actual

        // Buscar dos primos tal que p + q = n
        for (int p = 2; p <= n / 2; p++) {
            int q = n - p;
            if (esPrimo(p) && esPrimo(q)) {
                printf("Proceso %d: %d = %d + %d\n", id, n, p, q);
                break;  // Solo la primera combinación
            }
        }
    }

    MPI_Finalize();
    return 0;
}
