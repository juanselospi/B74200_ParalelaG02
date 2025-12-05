#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#define N 10000000   // 10 millones de elementos

int main() {

    // ------------------------------
    // Crear arreglo grande
    // ------------------------------
    int *A = new int[N];

    // inicializar arreglo con valores simples
    for (int i = 0; i < N; i++)
        A[i] = 1;   // para que la suma esperada sea N

    long sumaSerial = 0;
    long sumaOMP = 0;

    // ============================================================
    //               SUMA SERIAL (referencia)
    // ============================================================
    double t0 = omp_get_wtime();
    for (int i = 0; i < N; i++)
        sumaSerial += A[i];
    double t1 = omp_get_wtime();


    // ============================================================
    //          SUMA PARALELA CON OPENMP (reduction)
    // ============================================================
    double t2 = omp_get_wtime();

    #pragma omp parallel for reduction(+:sumaOMP)
    for (int i = 0; i < N; i++) {
        sumaOMP += A[i];   // Cada hilo acumula local → luego se combina
    }

    double t3 = omp_get_wtime();


    // ============================================================
    //                   MOSTRAR RESULTADOS
    // ============================================================
    printf("Suma serial      = %ld   (tiempo %.6f s)\n", sumaSerial, t1 - t0);
    printf("Suma con OpenMP  = %ld   (tiempo %.6f s)\n", sumaOMP, t3 - t2);

    delete[] A;
    return 0;
}
