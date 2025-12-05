/**
 *  Calcula el número PI utilizando la serie de Leibniz:
 *
 *     pi = sum_{i=0}^{n-1} (-1)^i * 4 / (2*i + 1)
 *
 *  Versión serial + versión paralela con OpenMP usando `atomic`.
 *
 *  Autor: (adaptado para OpenMP a partir de la versión con threads)
 *  Curso: Programación Paralela y Concurrente
 *
 *  Compilar:
 *      g++ -g -fopenmp PiPorSeriesOMP_atomic.cc -o PiPorSeriesOMP_atomic.out
 *
 *  Ejecutar:
 *      ./PiPorSeriesOMP_atomic.out [terminos] [hilos]
 */

#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/time.h>
#include <cmath>      // M_PI

// ---------------------------------------------------------------------
// Timer tipo "getTimer" como el de tus otros códigos
// ---------------------------------------------------------------------
void startTimer(struct timeval * timerStart) {
    gettimeofday(timerStart, NULL);
}

double getTimer(struct timeval timerStart) {
    struct timeval timerStop, timerElapsed;
    gettimeofday(&timerStop, NULL);
    timersub(&timerStop, &timerStart, &timerElapsed);
    return timerElapsed.tv_sec * 1000.0 + timerElapsed.tv_usec / 1000.0;
}

// ---------------------------------------------------------------------
// Versión SERIAL de la suma de la serie de PI
// ---------------------------------------------------------------------
double piSerial(long terminos) {

    double pi = 0.0;
    double signo = 4.0;  // empieza con +4

    for (long i = 0; i < terminos; i++) {
        long divisor = 2 * i + 1;    // 2*i + 1
        pi += signo / divisor;       // 4/(2*i+1) con signo alternante
        signo = -signo;              // cambia de +4 a -4 y viceversa
    }

    return pi;
}

// ---------------------------------------------------------------------
// Versión OpenMP usando acumulación global con `atomic`
// ---------------------------------------------------------------------
// Idea:
//
//  - Cada hilo recorre una parte de los términos de la serie.
//  - Calcula localmente la suma de sus términos.
//  - Luego **agrega** su suma local a una variable global usando
//    `#pragma omp atomic` para evitar race conditions.
//
// Nota: usamos `atomic` sólo UNA VEZ por hilo (al final), no en cada término,
//       para que sea mucho más eficiente.
// ---------------------------------------------------------------------
double piOMP_atomic(long terminos, int hilos) {

    double piGlobal = 0.0;  // acumulador compartido entre hilos

    #pragma omp parallel num_threads(hilos)
    {
        double piLocal = 0.0;    // acumulador local de cada hilo

        // Cada hilo va a iterar sobre diferentes valores de i
        #pragma omp for
        for (long i = 0; i < terminos; i++) {
            double signo = ( (i % 2) == 0 ) ? 4.0 : -4.0;  // (-1)^i * 4
            long divisor = 2 * i + 1;
            piLocal += signo / divisor;
        }

        // Acumular la suma local en la variable global
        // usando atomic para evitar data race.
        #pragma omp atomic
        piGlobal += piLocal;
    }

    return piGlobal;
}

// ---------------------------------------------------------------------
// MAIN
// ---------------------------------------------------------------------
int main(int argc, char **argv) {

    long terminos = 1000000;   // por defecto 1e6 términos
    int hilos = 8;             // por defecto 8 hilos

    if (argc > 1)
        terminos = atol(argv[1]);
    if (argc > 2)
        hilos = atoi(argv[2]);

    printf("\tValor de PI (math.h): %.20f\n", M_PI);
    printf("Calculando con %ld términos\n", terminos);
    printf("Usando %d hilos en la versión OpenMP.\n\n", hilos);

    struct timeval t;
    double elapsed;

    // ==========================
    //      VERSIÓN SERIAL
    // ==========================
    startTimer(&t);
    double piS = piSerial(terminos);
    elapsed = getTimer(t);

    printf("Serial PI:      %.20f   (tiempo = %.3f ms)\n", piS, elapsed);
    printf("Error absoluto: %.20f\n\n", fabs(piS - M_PI));


    // ==========================
    //      VERSIÓN OMP ATOMIC
    // ==========================
    startTimer(&t);
    double piP = piOMP_atomic(terminos, hilos);
    elapsed = getTimer(t);

    printf("OMP atomic PI:  %.20f   (tiempo = %.3f ms)\n", piP, elapsed);
    printf("Error absoluto: %.20f\n", fabs(piP - M_PI));

    return 0;
}
