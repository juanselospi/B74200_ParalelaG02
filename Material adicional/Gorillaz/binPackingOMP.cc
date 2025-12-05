/**
 *  PROBLEMA DEL CARRITO DE SUPERMERCADO — VERSIÓN OPENMP
 *
 *  Tenemos N artículos, cada uno con un PESO y un PRECIO.
 *  El carrito tiene una capacidad máxima K.
 *
 *  Queremos encontrar la mayor ganancia posible sin exceder la capacidad.
 *
 *  Estrategia usada (tipo examen parcial 1):
 *    1. Ordenar los artículos por precio descendente.
 *    2. Probar cada artículo como “primer artículo” a meter al carrito.
 *    3. Después meter otros artículos en orden de precio mientras quepan.
 *    4. Cada hilo procesa un subconjunto de artículos iniciales.
 *    5. Al final se combina la mejor ganancia encontrada por todos los hilos.
 *
 *  Paralelización:
 *    - Se usa omp parallel con división manual del trabajo.
 *    - Cada hilo calcula su mejor ganancia local.
 *    - Se combina usando una sección crítica.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <omp.h>

using namespace std;

/**
 * Función que resuelve el problema del carrito usando OpenMP
 *
 * @param capacidad    Capacidad máxima del carrito
 * @param pesos[]      Arreglo de pesos de los artículos
 * @param precios[]    Arreglo de precios de los artículos
 * @param hilos        Cantidad de hilos OpenMP a utilizar
 */
void carritoOpenMP(int capacidad, vector<int>& pesos, vector<int>& precios, int hilos) {

    int N = pesos.size();   // cantidad de artículos

    // ------------------------------
    // ORDENAR ARTÍCULOS POR PRECIO
    // ------------------------------

    vector<int> idx(N);    // idx contendrá posiciones ordenadas por precio
    for (int i = 0; i < N; i++)
        idx[i] = i;        // inicializo idx como [0,1,2,...]

    // Ordenar de mayor precio a menor
    sort(idx.begin(), idx.end(),
        [&](int a, int b) {
            return precios[a] > precios[b];
        });

    // ------------------------------
    // VARIABLES GLOBALES PARA RESULTADO
    // ------------------------------

    int mejorGlobalGanancia = 0;     // mejor ganancia encontrada
    int mejorGlobalCapacidad = 0;    // capacidad utilizada en esa solución

    // ------------------------------
    // CONFIGURAR CANTIDAD DE HILOS
    // ------------------------------

    omp_set_num_threads(hilos);

    // ------------------------------
    // BLOQUE PARALELO DE OPENMP
    // ------------------------------

    #pragma omp parallel
    {
        // Variables locales a cada hilo
        int mejorLocalGanancia = 0;
        int mejorLocalCapacidad = 0;

        int tid = omp_get_thread_num();    // id del hilo
        int T   = omp_get_num_threads();   // número total de hilos

        // Dividir el conjunto de artículos entre los hilos
        int inicio = tid * (N / T);
        int fin    = (tid == T - 1) ? N : inicio + (N / T);

        // Cada hilo probará como artículo inicial aquellos dentro de su rango
        for (int pos = inicio; pos < fin; pos++) {

            int itemInicial = idx[pos];       // es el artículo inicial que este hilo considera
            int capRestante = capacidad;      // capacidad temporal del carrito

            // Verificar si el primer artículo cabe en el carrito
            if (pesos[itemInicial] <= capRestante) {

                int ganancia = precios[itemInicial];     // ganancia inicial
                capRestante -= pesos[itemInicial];       // capacidad restante

                // Intentar meter otros artículos (que no sean el inicial)
                for (int j = 0; j < N; j++) {

                    if (j == pos) continue;              // el inicial no se repite

                    int otro = idx[j];                   // siguiente artículo por precio

                    // Si cabe, lo agrego
                    if (pesos[otro] <= capRestante) {
                        capRestante -= pesos[otro];
                        ganancia += precios[otro];
                    }
                }

                // Actualizar nuestro mejor resultado local
                if (ganancia > mejorLocalGanancia) {
                    mejorLocalGanancia = ganancia;
                    mejorLocalCapacidad = capacidad - capRestante;
                }
            }
        }

        // -----------------------------------------
        // ACTUALIZAR EL MEJOR RESULTADO GLOBAL
        // -----------------------------------------

        #pragma omp critical   // solo un hilo a la vez puede entrar aquí
        {
            if (mejorLocalGanancia > mejorGlobalGanancia) {
                mejorGlobalGanancia = mejorLocalGanancia;
                mejorGlobalCapacidad = mejorLocalCapacidad;
            }
        }
    }

    // ------------------------------
    // IMPRIMIR RESULTADO FINAL
    // ------------------------------

    cout << "\n[OpenMP] Mejor ganancia = " << mejorGlobalGanancia
         << " usando capacidad = " << mejorGlobalCapacidad << " de " << capacidad << "\n";
}



/**
 * ================================
 *               MAIN
 * ================================
 */
int main() {

    // Ejemplo del examen parcial:
    int capacidad = 10;

    // Pesos de los artículos
    vector<int> pesos = {1, 2, 3, 8, 7, 4};

    // Precios correspondientes
    vector<int> precios = {20, 5, 10, 40, 15, 25};

    int hilos = 4;

    cout << "=== PROBLEMA DEL CARRITO CON OPENMP ===\n";
    cout << "Capacidad del carrito = " << capacidad << "\n";
    cout << "Artículos = " << pesos.size() << "\n";

    carritoOpenMP(capacidad, pesos, precios, hilos);

    return 0;
}
