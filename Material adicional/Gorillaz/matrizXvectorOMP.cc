#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#define N 20000   // tamaño del vector y matriz (puedes ajustar)

// ------------------------------------------------------------
// Llena matriz y vector con valores simples
// ------------------------------------------------------------
void llenar(double *A, double *x) {
    for (int i = 0; i < N*N; i++)
        A[i] = rand() % 10;

    for (int i = 0; i < N; i++)
        x[i] = rand() % 10;
}

// ------------------------------------------------------------
// y = A * x   (versión serial)
// ------------------------------------------------------------
void matVecSerial(double *A, double *x, double *y) {

    for (int i = 0; i < N; i++) {

        double suma = 0.0;

        for (int k = 0; k < N; k++)
            suma += A[i*N + k] * x[k];

        y[i] = suma;
    }
}

// ------------------------------------------------------------
// y = A * x   usando OpenMP
// Paraleliza por filas → cada hilo calcula y[i]
// ------------------------------------------------------------
void matVecOMP(double *A, double *x, double *y) {

    #pragma omp parallel for
    for (int i = 0; i < N; i++) {

        double suma = 0.0;

        for (int k = 0; k < N; k++)
            suma += A[i*N + k] * x[k];

        y[i] = suma;
    }
}

// ------------------------------------------------------------
// Versión alternativa con schedule(dynamic)
// útil si cada fila requiere distinto trabajo
// ------------------------------------------------------------
void matVecOMP_dynamic(double *A, double *x, double *y) {

    #pragma omp parallel for schedule(dynamic, 50)
    for (int i = 0; i < N; i++) {

        double suma = 0.0;

        for (int k = 0; k < N; k++)
            suma += A[i*N + k] * x[k];

        y[i] = suma;
    }
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------
int main() {

    srand(time(NULL));

    double *A = new double[N*N];
    double *x = new double[N];
    double *y = new double[N];
    double *z = new double[N];

    llenar(A, x);

    double t0, t1;

    // =========================
    //     SERIAL
    // =========================
    t0 = omp_get_wtime();
    matVecSerial(A, x, y);
    t1 = omp_get_wtime();
    printf("Serial: %.6f s\n", t1 - t0);

    // =========================
    //     OpenMP
    // =========================
    t0 = omp_get_wtime();
    matVecOMP(A, x, z);
    t1 = omp_get_wtime();
    printf("OpenMP: %.6f s\n", t1 - t0);

    // Validación simple
    for (int i = 0; i < 10; i++)
        printf("y[%d]=%.2f   z[%d]=%.2f\n", i, y[i], i, z[i]);

    delete[] A;
    delete[] x;
    delete[] y;
    delete[] z;

    return 0;
}
