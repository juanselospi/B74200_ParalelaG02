/**
 * ================================================================
 *     PROBLEMA DEL CARRITO (BIN PACKING / KNAPSACK APROXIMADO)
 *              *** Versión MPI (un solo archivo) ***
 * ================================================================
 *
 * ENUNCIADO:
 * Se tiene una tienda con N artículos. Cada artículo tiene:
 *      - un PESO
 *      - un PRECIO (ganancia)
 *
 * La meta es llenar un carrito con capacidad K para maximizar
 * la ganancia, sin pasarse de peso.
 *
 * Estrategia utilizada (la misma del examen 1):
 * 1) Ordenar los artículos de mayor precio → menor precio.
 * 2) Para cada artículo posible como primer ítem del carrito:
 *        - Si cabe, se agrega.
 *        - Luego se intenta llenar el carrito con los artículos
 *          más caros restantes mientras haya espacio.
 * 3) La mejor ganancia encontrada es la respuesta.
 *
 * Versión MPI:
 *   - Se divide el rango de ítems iniciales entre procesos.
 *   - Cada proceso calcula la mejor ganancia de SU rango.
 *   - Rank 0 recolecta los resultados con MPI_Reduce.
 *
 * COMPILAR:
 *      mpic++ -g carritoMPI.cc -o carritoMPI.out
 *
 * EJECUTAR:
 *      mpirun -np 4 ./carritoMPI.out
 *
 * ================================================================
 */

#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
using namespace std;

// ---------------------------------------------------------------
// Datos del problema (mismo dataset del examen)
// ---------------------------------------------------------------
const int K = 10;                  // Capacidad del carrito
const int N = 6;                   // Cantidad de artículos

int pesos[N]  = { 1,  2,  3,  8,  7,  4 };
int precios[N] = {20, 5, 10, 40, 15, 25};


// ---------------------------------------------------------------
// Función auxiliar: imprime el arreglo
// ---------------------------------------------------------------
void imprimirDatos() {
    printf("Artículos disponibles:\n");
    for (int i = 0; i < N; i++)
        printf(" Item %d -> peso=%d precio=%d\n", i, pesos[i], precios[i]);
    printf("\n");
}


// ---------------------------------------------------------------
// Lógica para intentar llenar un carrito iniciando en un ítem base
// (MISMA lógica de tu versión serial y OpenMP)
// ---------------------------------------------------------------
int evaluarCarrito(int firstItem, int* orden) {

    int remaining = K;
    int profit = 0;

    // Intento agregar el primer item
    int f = orden[firstItem];

    if (pesos[f] > remaining)
        return 0;   // no cabe → ganancia 0

    remaining -= pesos[f];
    profit += precios[f];

    // Intento agregar otros ítems (si caben)
    for (int j = 0; j < N; j++) {
        if (j == firstItem) continue; // no repetir
        int item = orden[j];
        if (pesos[item] <= remaining) {
            remaining -= pesos[item];
            profit += precios[item];
        }
    }
    return profit;
}


// ==============================================================
//                          MAIN MPI
// ==============================================================
int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // id del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &size); // número total de procesos

    if (rank == 0) {
        printf("\n=== PROBLEMA DEL CARRITO — MPI ===\n");
        printf("Capacidad del carrito = %d\n", K);
        imprimirDatos();
    }

    // ------------------------------------------------------------
    // Paso 1: Crear el arreglo "orden" que ordena ítems por precio
    // ------------------------------------------------------------
    int orden[N];
    for (int i = 0; i < N; i++) orden[i] = i;

    // Orden descendente por precio
    sort(orden, orden + N, [&](int a, int b) {
        return precios[a] > precios[b];
    });

    // ------------------------------------------------------------
    // Paso 2: Dividir trabajo entre procesos MPI
    // ------------------------------------------------------------
    int chunk = N / size;              // cuántos ítems por proceso
    int resto = N % size;              // sobran algunos → se reparten uno extra

    int start, finish;

    if (rank < resto) {
        start = rank * (chunk + 1);
        finish = start + (chunk + 1);
    } else {
        start = rank * chunk + resto;
        finish = start + chunk;
    }

    // ------------------------------------------------------------
    // Paso 3: Cada proceso evalúa su subconjunto de ítems iniciales
    // ------------------------------------------------------------
    int bestLocal = 0; // mejor ganancia local

    for (int i = start; i < finish; i++) {
        int profit = evaluarCarrito(i, orden);
        if (profit > bestLocal)
            bestLocal = profit;
    }

    // ------------------------------------------------------------
    // Paso 4: Reducir al mejor resultado global
    // ------------------------------------------------------------
    int bestGlobal = 0;

    MPI_Reduce(&bestLocal,    // valor local
               &bestGlobal,   // valor global
               1,             // cantidad
               MPI_INT,       // tipo
               MPI_MAX,       // operación reducción
               0,             // root = proceso 0
               MPI_COMM_WORLD);

    // ------------------------------------------------------------
    // Paso 5: Rank 0 imprime resultado final
    // ------------------------------------------------------------
    if (rank == 0) {
        printf("[MPI] Mejor ganancia = %d\n\n", bestGlobal);
    }

    MPI_Finalize();
    return 0;
}
