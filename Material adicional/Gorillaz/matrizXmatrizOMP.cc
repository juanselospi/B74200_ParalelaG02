#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#define N 600   // tamaño de la matriz NxN

// ------------------------------------------------------------
// Inicializa una matriz NxN con valores aleatorios
// ------------------------------------------------------------
void llenarMatriz(double *M) {
    for (int i = 0; i < N*N; i++)
        M[i] = rand() % 10;
}

// ------------------------------------------------------------
// C = A * B   (multiplicación matricial serial)
// ------------------------------------------------------------
void multiplicarSerial(double *A, double *B, double *C) {

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {

            double suma = 0.0;
            for (int k = 0; k < N; k++)
                suma += A[i*N + k] * B[k*N + j];

            C[i*N + j] = suma;
        }
}

// ------------------------------------------------------------
// C = A * B   usando OpenMP parallel for
// Cada hilo recibe filas distintas
// ------------------------------------------------------------
void multiplicarOMP(double *A, double *B, double *C) {

    #pragma omp parallel for
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {

            double suma = 0.0;
            for (int k = 0; k < N; k++)
                suma += A[i*N + k] * B[k*N + j];

            C[i*N + j] = suma;
        }
}

// ------------------------------------------------------------
// Versión más rápida: collapse(2)
// Le dice a OpenMP que combine los loops i y j
// para repartir mejor el trabajo cuando hay muchos hilos
// ------------------------------------------------------------
void multiplicarOMP_collapse(double *A, double *B, double *C) {

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {

            double suma = 0.0;
            for (int k = 0; k < N; k++)
                suma += A[i*N + k] * B[k*N + j];

            C[i*N + j] = suma;
        }
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------
int main() {

    srand(time(NULL));

    // Matrices en arreglo plano (más rápido que usar double**)
    double *A = new double[N*N];
    double *B = new double[N*N];
    double *C = new double[N*N];
    double *D = new double[N*N];
    double *E = new double[N*N];

    llenarMatriz(A);
    llenarMatriz(B);

    double t0, t1;

    // =========================
    //     SERIAL
    // =========================
    t0 = omp_get_wtime();
    multiplicarSerial(A, B, C);
    t1 = omp_get_wtime();
    printf("Tiempo serial:   %.6f s\n", t1 - t0);

    // =========================
    //     OPENMP simple
    // =========================
    t0 = omp_get_wtime();
    multiplicarOMP(A, B, D);
    t1 = omp_get_wtime();
    printf("OpenMP simple:   %.6f s\n", t1 - t0);

    // =========================
    //     OPENMP collapse(2)
    // =========================
    t0 = omp_get_wtime();
    multiplicarOMP_collapse(A, B, E);
    t1 = omp_get_wtime();
    printf("OpenMP collapse: %.6f s\n", t1 - t0);

    delete[] A;
    delete[] B;
    delete[] C;
    delete[] D;
    delete[] E;

    return 0;
}
