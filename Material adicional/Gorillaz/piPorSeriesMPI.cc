/**
 * ============================================================
 *      Cálculo del número PI usando la serie de Leibniz
 *              Versión distribuida con MPI
 *
 *      Fórmula:
 *           π = Σ ( i = 0 → n-1 ) [ 4 * (-1)^i / (2*i + 1) ]
 *
 *      Objetivo:
 *      - Dividir las iteraciones entre procesos MPI
 *      - Cada proceso calcula su suma parcial de la serie
 *      - Se combinan los resultados usando MPI_Reduce con MPI_SUM
 *
 *      Este es un ejercicio MUY típico de examen:
 *      ✔ División de trabajo
 *      ✔ Cálculo parcial independiente
 *      ✔ Comunicación final vía reduce
 *
 * ============================================================
 */

#include <mpi.h>
#include <cstdio>
#include <cmath>

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int id, nproc;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);     // Identificador del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);  // Número total de procesos

    // ============================================================
    //   1) Configurar número de iteraciones
    // ============================================================
    long N = 1000000;        // Iteraciones por defecto

    if (argc > 1)
        N = atol(argv[1]);   // Permite cambiarlo por terminal

    // ============================================================
    //   2) División del trabajo entre procesos
    // ============================================================
    long chunk = N / nproc;               // Cuántos términos calcula cada proceso
    long inicio = id * chunk;             // Primer término que calcula este proceso
    long fin = inicio + chunk;            // Último (NO incluido)

    // Si la división no es exacta, el último proceso calcula más
    if (id == nproc - 1)
        fin = N;

    // ============================================================
    //   3) Cada proceso calcula su suma parcial
    // ============================================================
    double sumaLocal = 0.0;

    for (long i = inicio; i < fin; i++)
    {
        double signo = (i % 2 == 0) ? 4.0 : -4.0;    // Alternancia +4, -4
        double termino = signo / (2.0 * i + 1.0);    // Fórmula 4(-1)^i / (2i+1)
        sumaLocal += termino;
    }

    // ============================================================
    //   4) Reducir los resultados en el proceso 0
    // ============================================================
    double piTotal = 0;

    MPI_Reduce(
        &sumaLocal,         // Valor parcial local
        &piTotal,           // Valor total en proceso 0
        1,                  // Cantidad de elementos a reducir
        MPI_DOUBLE,         // Tipo de dato
        MPI_SUM,            // Operación (suma)
        0,                  // Proceso raíz
        MPI_COMM_WORLD      // Communicator global
    );

    // ============================================================
    //   5) Proceso 0 imprime el resultado final
    // ============================================================
    if (id == 0)
    {
        printf("\n============================\n");
        printf("  Aproximación de PI con MPI\n");
        printf("============================\n");
        printf("Iteraciones totales: %ld\n", N);
        printf("Procesos utilizados: %d\n", nproc);
        printf("Resultado aproximado: %.15f\n", piTotal);
        printf("Valor real (M_PI):   %.15f\n", M_PI);
        printf("Error absoluto:      %.15f\n\n", fabs(piTotal - M_PI));
    }

    MPI_Finalize();
    return 0;
}
