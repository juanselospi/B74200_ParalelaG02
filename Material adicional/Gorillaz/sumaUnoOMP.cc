/**
 *  SumaUno con OpenMP – Versión usando reduction
 *
 *  Equivalente al ejemplo con pthreads, pero usando:
 *
 *      #pragma omp parallel for reduction(+:total)
 *
 *  Esta es la forma más limpia y eficiente en OpenMP para evitar data races.
 *
 *  Compilar:
 *      g++ -g -fopenmp sumaUnoReduction.cc -o sumaUnoReduction.out
 */

#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#define ITER 1000    // cada hilo hace 1000 incrementos

long total = 0;


// ============================================================
// VERSIÓN SERIAL (para comparación)
// ============================================================
long SerialTest(long hilos)
{
    total = 0;

    for (long h = 0; h < hilos; h++) {
        for (int i = 0; i < ITER; i++) {
            total++;
            usleep(1);
        }
    }

    return total;
}


// ============================================================
// VERSIÓN OPENMP CON REDUCTION
// ============================================================
long OMP_Reduction(long hilos)
{
    total = 0;

    // Esta es la magia:
    //    - Cada iteración del FOR es independiente
    //    - Cada iteración suma a su propio total local
    //    - OpenMP combina todos los totales al final
    #pragma omp parallel for num_threads(hilos) reduction(+:total)
    for (long h = 0; h < hilos; h++) {

        long local = 0;  // cada hilo acumula aquí

        for (int i = 0; i < ITER; i++) {
            local++;
            usleep(1);
        }

        total += local;  // este total es reducido automáticamente
    }

    return total;
}


// ============================================================
// MAIN
// ============================================================
int main(int argc, char** argv)
{
    long hilos = 100;

    if (argc > 1)
        hilos = atol(argv[1]);

    double t0, t1;

    // SERIAL
    t0 = omp_get_wtime();
    long r1 = SerialTest(hilos);
    t1 = omp_get_wtime();
    printf("Serial version:    total=%ld, time=%f s\n", r1, t1 - t0);

    // REDUCTION
    t0 = omp_get_wtime();
    long r2 = OMP_Reduction(hilos);
    t1 = omp_get_wtime();
    printf("OMP reduction:     total=%ld, time=%f s\n", r2, t1 - t0);

    return 0;
}
