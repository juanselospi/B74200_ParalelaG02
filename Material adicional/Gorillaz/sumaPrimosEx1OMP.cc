/**
 * ================================================================
 *  Problema de Goldbach con OpenMP
 * ================================================================
 *
 * Enunciado:
 *   Dado un número N (par y mayor o igual que 6), queremos considerar
 *   todos los números pares desde 6 hasta N:
 *
 *       6, 8, 10, 12, ..., N
 *
 *   Para cada número par "num", queremos encontrar dos números primos p y q
 *   tales que:
 *
 *         p + q = num
 *
 *   Esto está basado en la Conjetura de Goldbach.
 *
 *   Objetivo:
 *     - Versión con OpenMP donde:
 *       * Cada hilo toma una parte del arreglo de pares.
 *       * Cada hilo busca los dos primos para sus elementos.
 *       * Se imprimen los resultados indicando qué hilo los calculó.
 *
 *   Notas:
 *     - Es un excelente ejercicio de examen porque cada número par
 *       puede calcularse de forma independiente → perfecto para parallel for.
 *     - No hay memoria compartida complicada más allá de lectura del arreglo
 *       y escribir en pantalla (donde usamos una sección crítica).
 *
 * ================================================================
 */

#include <cstdio>    // printf
#include <cstdlib>   // atoi
#include <vector>    // std::vector
#include <omp.h>     // OpenMP

// ------------------------------------------------------------
// Función auxiliar: verifica si un número es primo
// ------------------------------------------------------------
bool esPrimo(int x) {
    // Números menores que 2 NO son primos
    if (x < 2) return false;

    // Si es par, solo el 2 es primo
    if (x % 2 == 0) return x == 2;

    // Probar divisores impares desde 3 hasta sqrt(x)
    for (int d = 3; d * d <= x; d += 2) {
        if (x % d == 0)  // Si es divisible por d, no es primo
            return false;
    }

    // Si no encontramos divisor, es primo
    return true;
}

// ------------------------------------------------------------
// Dado un número par "num", encuentra p y q primos tales que:
//      p + q = num
// ------------------------------------------------------------
void goldbach(int num, int &p, int &q) {
    // Recorremos posibles p desde 2 hasta num/2
    // (más allá de num/2 sería simétrico: num-p ya se revisó)
    for (int a = 2; a <= num / 2; a++) {

        // Si 'a' es primo, y 'num - a' también lo es, encontramos la pareja
        if (esPrimo(a) && esPrimo(num - a)) {
            p = a;           // Guardamos el primer primo
            q = num - a;     // Y el segundo primo
            return;          // Terminamos la función
        }
    }

    // Teóricamente no deberíamos llegar aquí para números pares >= 6
    // pero por seguridad podríamos poner algún valor por defecto
    p = q = -1;
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------
int main(int argc, char** argv) {
    // N será el límite superior de los pares.
    // Por defecto usamos N = 50.
    int N = 50;

    // Si el usuario pasa un argumento, lo usamos como N:
    //   ./goldbachOMP 100
    if (argc > 1) {
        N = atoi(argv[1]);
    }

    // Aseguramos que N sea par y al menos 6.
    if (N < 6) N = 6;
    if (N % 2 != 0) N++;   // Si N es impar, lo hacemos par sumando 1.

    // Construimos un vector con todos los pares desde 6 hasta N.
    std::vector<int> pares;
    for (int num = 6; num <= N; num += 2) {
        pares.push_back(num);
    }

    // Imprimimos cuántos pares vamos a procesar.
    printf("=== GOLD BACH con OpenMP ===\n");
    printf("Procesando pares desde 6 hasta %d (total = %zu)\n\n", N, pares.size());

    // --------------------------------------------------------
    // Región paralela con for dividido entre hilos
    // --------------------------------------------------------
    // Cada iteración del for (cada par) es independiente, por eso
    // es perfecto para #pragma omp parallel for.
    // --------------------------------------------------------
    #pragma omp parallel for
    for (int i = 0; i < (int)pares.size(); i++) {

        // Obtenemos el número par que le toca a esta iteración
        int num = pares[i];

        // Variables locales para guardar la pareja de primos
        int p, q;

        // Calculamos los primos p y q tales que p + q = num
        goldbach(num, p, q);

        // Obtenemos el id del hilo que está ejecutando esta iteración
        int tid = omp_get_thread_num();

        // Sección crítica para imprimir:
        // evitamos que dos hilos hagan printf al mismo tiempo y mezclen texto.
        #pragma omp critical
        {
            printf("Hilo %d: %d = %d + %d\n", tid, num, p, q);
        }
    }

    // Mensaje final para ver que terminó el programa
    printf("\nFin de Goldbach con OpenMP.\n");

    return 0;
}
