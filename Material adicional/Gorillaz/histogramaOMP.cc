#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#define N 20000000     // Tamaño del arreglo. Muy grande para justificar paralelismo.
#define RANGOS 10      // Valores posibles en el arreglo (0..9)

// ================================================================
// FUNCIÓN PARA LLENAR EL ARREGLO
// ================================================================
// Ponemos valores aleatorios entre 0 y RANGOS-1.
// El histograma contará cuántas veces aparece cada valor.
void llenar(int *A) {
    for (int i = 0; i < N; i++)
        A[i] = rand() % RANGOS;
}

// ================================================================
// HISTOGRAMA SERIAL (REFERENCIA)
// ================================================================
// Implementación tradicional SIN paralelismo.
// Se usa como baseline para comprobar que la versión paralela
// produce exactamente los mismos resultados.
void histogramaSerial(int *A, int *hist) {

    // Inicializamos las casillas en 0.
    for (int i = 0; i < RANGOS; i++)
        hist[i] = 0;

    // Contamos cada valor.
    for (int i = 0; i < N; i++)
        hist[A[i]]++;
}

// ================================================================
// HISTOGRAMA PARALELO CON OPENMP
// ================================================================
// IDEA CLAVE (IMPORTANTE PARA EL EXAMEN):
//   NO podemos hacer:
//
//       #pragma omp parallel for
//       hist[A[i]]++;
//
//   porque todos los hilos estarían escribiendo sobre el mismo arreglo,
//   y eso genera una condición de carrera (race condition).
//
// SOLUCIÓN CORRECTA Y MUY CLÁSICA:
//   * Cada hilo crea SU PROPIO histograma local.
//   * Cada hilo hace su conteo sin interferir con los demás.
//   * Luego se combinan los histogramas locales en un histograma global.
// ================================================================
void histogramaOMP(int *A, int *histGlobal) {

    // Poner histograma global en 0 (no paralelo por simplicidad).
    for (int i = 0; i < RANGOS; i++)
        histGlobal[i] = 0;

    int numHilos = 0;

    // Comienza región paralela.
    #pragma omp parallel
    {
        // Solo un hilo ejecuta este bloque: obtiene el número real de hilos.
        #pragma omp single
        numHilos = omp_get_num_threads();

        // ------------------------------------------------------------
        // Histograma privado POR HILO.
        // IMPORTANTE:
        //   * Esto evita condiciones de carrera.
        //   * Cada hilo modifica su arreglo privado sin interferir.
        // ------------------------------------------------------------
        int histLocal[RANGOS] = {0};   // Inicializado a 0

        // ------------------------------------------------------------
        // Cada hilo procesa una parte del arreglo A.
        // OpenMP divide el for automáticamente entre los hilos.
        // ------------------------------------------------------------
        #pragma omp for
        for (int i = 0; i < N; i++)
            histLocal[A[i]]++;  
            // Aquí NO hay race condition porque histLocal es privado por hilo.

        // ------------------------------------------------------------
        // FASE DE REDUCCIÓN MANUAL
        // ------------------------------------------------------------
        // Ahora sumamos cada histLocal al histograma global.
        // Se usa "critical" para evitar que dos hilos modifiquen histGlobal
        // al mismo tiempo.
        #pragma omp critical
        {
            for (int r = 0; r < RANGOS; r++)
                histGlobal[r] += histLocal[r];
        }
    }

    printf("Se usaron %d hilos.\n", numHilos);
}


// ================================================================
// MAIN
// ================================================================
int main() {

    srand(time(NULL));

    // Arreglo grande y histogramas
    int *A = new int[N];
    int histS[RANGOS];  // histograma serial
    int histP[RANGOS];  // histograma paralelo

    llenar(A);

    double t0, t1;

    // ============================
    //      SERIAL
    // ============================
    t0 = omp_get_wtime();
    histogramaSerial(A, histS);
    t1 = omp_get_wtime();
    printf("Tiempo serial:   %.6f s\n", t1 - t0);

    // ============================
    //      OPENMP
    // ============================
    t0 = omp_get_wtime();
    histogramaOMP(A, histP);
    t1 = omp_get_wtime();
    printf("Tiempo OMP:      %.6f s\n", t1 - t0);

    // ============================
    // COMPROBACIÓN DE CORRECCIÓN
    // ============================
    printf("\nHistograma final:\n");
    for (int r = 0; r < RANGOS; r++)
        printf("Valor %d → Serial: %d   |   Paralelo: %d\n",
               r, histS[r], histP[r]);

    delete[] A;
    return 0;
}

/*
Es un conteo de frecuencias: dado un arreglo de valores discretos, el histograma dice cuántas veces aparece cada valor. 
Para paralelizarlo en OpenMP necesitamos que cada hilo use un histograma privado y luego lo combine en el histograma global para evitar condiciones de carrera.
*/