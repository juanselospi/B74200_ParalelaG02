/**
 * Cálculo del producto punto (dot product) entre dos vectores usando OpenMP.
 *
 *   dot = sum( A[i] * B[i] )  para i = 0..N-1
 *
 * Elementos típicos que un profesor evalúa:
 *   - paralelo con #pragma omp parallel for
 *   - uso correcto de reduction para evitar data race
 *   - inicialización y verificación con la versión serial
 */

#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#define N 10000000   // tamaño de los vectores

int main() {
    // ---------------------------------------------
    // 1. Reservar memoria para los vectores
    // ---------------------------------------------
    double* A = (double*) malloc(sizeof(double) * N);
    double* B = (double*) malloc(sizeof(double) * N);

    // Inicializamos valores aleatorios
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        A[i] = rand() % 10;   // valores entre 0 y 9
        B[i] = rand() % 10;
    }

    // ---------------------------------------------
    // 2. Producto punto SERIAL (referencia)
    // ---------------------------------------------
    double dotSerial = 0.0;
    double t0 = omp_get_wtime();

    for (int i = 0; i < N; i++)
        dotSerial += A[i] * B[i];

    double t1 = omp_get_wtime();

    printf("Serial:  dot = %.2f   tiempo = %.5f s\n",
           dotSerial, t1 - t0);


    // ---------------------------------------------
    // 3. Producto punto con OpenMP (paralelo)
    // ---------------------------------------------
    double dotOMP = 0.0;
    double t2 = omp_get_wtime();

    /**
     * parallel for:
     *   Cada hilo procesa una parte del vector.
     *
     * reduction(+:dotOMP):
     *   Cada hilo acumula localmente su parte
     *   y OpenMP combina todas las sumas al final.
     *
     * ¡Evita race condition!
     */
    #pragma omp parallel for reduction(+:dotOMP)
    for (int i = 0; i < N; i++) {
        dotOMP += A[i] * B[i];
    }

    double t3 = omp_get_wtime();

    printf("OpenMP:  dot = %.2f   tiempo = %.5f s\n",
           dotOMP, t3 - t2);


    // ---------------------------------------------
    // 4. Verificación
    // ---------------------------------------------
    if (dotSerial == dotOMP)
        printf("Resultado CORRECTO ✓\n");
    else
        printf("Resultado INCORRECTO ✗\n");


    // ---------------------------------------------
    // 5. Liberar memoria
    // ---------------------------------------------
    free(A);
    free(B);

    return 0;
}
