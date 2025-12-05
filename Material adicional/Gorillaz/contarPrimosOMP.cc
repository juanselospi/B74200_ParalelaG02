/**
 * Contar cuántos números primos hay en el rango [1, N]
 * usando OpenMP.
 *
 * Características importantes del examen:
 *  - parallel for    → divide el trabajo entre hilos
 *  - private(i)      → evitar condiciones de carrera
 *  - reduction(+:contador) → acumular resultados sin critical
 *  - schedule(dynamic) → balancear la carga (primos son costosos)
 */

#include <omp.h>
#include <cstdio>
#include <cmath>

// ================================================
// Función para verificar si un número es primo
// (versión simple y típica de exámenes)
// ================================================
bool esPrimo(long n) {
    if (n < 2) return false;         // 0 y 1 NO son primos
    if (n % 2 == 0 && n != 2) return false;

    long limite = std::sqrt(n);
    for (long i = 3; i <= limite; i += 2)
        if (n % i == 0)
            return false;

    return true;
}

// ================================================
// MAIN: versión OpenMP
// ================================================
int main() {
    long N = 200000;   // número límite (modificable)
    int totalPrimos = 0;

    // --------------------------------------------
    // PARALelización del bucle con OpenMP
    // --------------------------------------------
    #pragma omp parallel for reduction(+:totalPrimos) schedule(dynamic, 100)
    for (long num = 1; num <= N; num++) {
        if (esPrimo(num))
            totalPrimos++;
    }

    printf("Cantidad de números primos entre 1 y %ld = %d\n",
           N, totalPrimos);

    return 0;
}
