/**
 * Buscar el número mayor de un arreglo usando OpenMP.
 *
 * Cada hilo procesa una SECCIÓN manual del arreglo:
 *   inicio = tid * (N / numHilos)
 *   fin    = (tid == numHilos - 1) ? N : inicio + (N / numHilos)
 *
 * Cada hilo encuentra su máximo local.
 * Luego compara contra el máximo global (maxNum) usando:
 *      #pragma omp atomic write / read
 *
 * Esto evita condiciones de carrera.
 */

#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#define N 100   // tamaño del arreglo

int main() {

    int A[N];
    int maxNum = -999999;   // valor global inicial muy bajo
    int numHilos = 4;

    srand(time(NULL));

    // ================================
    // Generar arreglo aleatorio
    // ================================
    printf("Arreglo generado:\n");
    for (int i = 0; i < N; i++) {
        A[i] = rand() % 1000;   // valores de 0 a 999
        printf("%d ", A[i]);
    }
    printf("\n\n");

    // ================================
    // Paralelizar búsqueda del máximo
    // ================================
    #pragma omp parallel num_threads(numHilos)
    {
        int tid = omp_get_thread_num();

        // Calcular sección para este hilo
        int bloque = N / numHilos;
        int inicio = tid * bloque;
        int fin = (tid == numHilos - 1) ? N : inicio + bloque;

        // Máximo local del hilo
        int localMax = -999999;

        for (int i = inicio; i < fin; i++) {
            if (A[i] > localMax)
                localMax = A[i];
        }

        printf("Hilo %d encontró maxLocal = %d en rango [%d, %d)\n",
               tid, localMax, inicio, fin);

        // -----------------------------------------
        // Actualizar máximo global con atomic
        // -----------------------------------------
        int tempGlobal;

        // Primero leer el valor actual
        #pragma omp atomic read
        tempGlobal = maxNum;

        // Si el máximo local lo supera, intentar escribirlo
        if (localMax > tempGlobal) {
            #pragma omp atomic write
            maxNum = localMax;
        }
    }

    printf("\n========================\n");
    printf("Máximo global encontrado: %d\n", maxNum);
    printf("========================\n");

    return 0;
}
