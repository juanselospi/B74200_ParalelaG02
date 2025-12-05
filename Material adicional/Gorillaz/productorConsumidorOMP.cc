/**
 * Problema Productor–Consumidor usando OpenMP con locks.
 *
 * - Buffer circular de tamaño BUFFER_SIZE
 * - Hilos pares  = productores
 * - Hilos impares = consumidores
 * - Exclusión mutua con omp_lock_t (lockBuffer)
 * - Sin `return` dentro de regiones paralelas (usamos break)
 *
 * Esta versión es ideal para examen porque:
 *   - Muestra uso correcto de locks OpenMP
 *   - Coordina bien productores y consumidores
 *   - No duplica ítems ni consume basura
 *   - Termina sola cuando se han producido y consumido todos los ítems
 */

#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#define BUFFER_SIZE 5       // tamaño del buffer circular
#define TOTAL_ITEMS 20      // número total de ítems a producir

// ---------------------------------------------------------
// VARIABLES COMPARTIDAS
// ---------------------------------------------------------
int buffer[BUFFER_SIZE];    // buffer circular
int inPos = 0;              // índice de escritura (productor)
int outPos = 0;             // índice de lectura (consumidor)
int countItems = 0;         // cuántos ítems hay en el buffer

int produced = 0;           // cuántos ítems se han producido
int consumed = 0;           // cuántos ítems se han consumido

// Lock global que protege TODAS las variables de arriba
omp_lock_t lockBuffer;

// ==================================================================
// FUNCIÓN PRODUCTOR
// ==================================================================
void productor(int tid)
{
    while (true) {

        int debeSalir = 0;   // bandera local para saber si salgo del while
        int produjoAlgo = 0; // bandera para saber si efectivamente produje

        // -----------------------------------------
        // SECCIÓN CRÍTICA PROTEGIDA POR LOCK
        // -----------------------------------------
        omp_set_lock(&lockBuffer);

        // ¿Ya se produjeron TODOS los ítems?
        if (produced >= TOTAL_ITEMS) {
            // Sí → este hilo productor ya no tiene nada más que hacer
            debeSalir = 1;
        } 
        else if (countItems < BUFFER_SIZE) {
            // Hay espacio en el buffer → puedo producir

            int item = produced;  // el próximo ítem a crear
            produced++;           // incremento el contador global de producción

            // Escribir en el buffer circular
            buffer[inPos] = item;
            inPos = (inPos + 1) % BUFFER_SIZE;
            countItems++;         // ahora hay un ítem más en el buffer

            produjoAlgo = 1;      // marcar que sí se produjo algo

            printf("Productor %d produjo %d (items en buffer=%d)\n",
                   tid, item, countItems);
        }
        // else:
        //   - produced < TOTAL_ITEMS pero el buffer está lleno
        //   - no puedo producir nada en esta iteración

        omp_unset_lock(&lockBuffer); // liberar el lock ANTES de decidir qué hacer

        // Si ya no hay nada que producir → salir del ciclo
        if (debeSalir)
            break;

        // Si no pude producir (buffer estaba lleno), descanso un poco
        if (!produjoAlgo) {
            usleep(1000); // small backoff para no pegarle tan duro a la CPU
        }

        // Si sí produjo, vuelve al while y lo intenta de nuevo
    }
}


// ==================================================================
// FUNCIÓN CONSUMIDOR
// ==================================================================
void consumidor(int tid)
{
    while (true) {

        int debeSalir = 0;    // bandera para salir del while
        int consumioAlgo = 0; // bandera para saber si consumió algo
        int item = -1;        // ítem consumido (si aplica)

        // -----------------------------------------
        // SECCIÓN CRÍTICA PROTEGIDA POR LOCK
        // -----------------------------------------
        omp_set_lock(&lockBuffer);

        // Condición de salida: ya se consumió TODO
        // y además el buffer está vacío.
        if (consumed >= TOTAL_ITEMS && countItems == 0) {
            debeSalir = 1;
        }
        else if (countItems > 0) {
            // Hay algo que consumir

            item = buffer[outPos];         // tomar el ítem del buffer
            outPos = (outPos + 1) % BUFFER_SIZE;
            countItems--;                  // hay un item menos en el buffer
            consumed++;                    // aumentamos contador global de consumo

            consumioAlgo = 1;

            // Nota: NO imprimimos dentro del lock para minimizar tiempo bloqueado
        }
        // else:
        //   - countItems == 0 PERO todavía no hemos consumido TOTAL_ITEMS
        //   - puede que un productor genere algo luego

        omp_unset_lock(&lockBuffer); // liberamos el lock

        if (debeSalir)
            break;   // no hay nada más que consumir jamás → salimos

        if (consumioAlgo) {
            // Ahora sí podemos imprimir (fuera del lock)
            printf("Consumidor %d consumió %d (items en buffer=%d)\n",
                   tid, item, countItems);
        } else {
            // No había nada para consumir en este momento → descansar un poco
            usleep(1000);
        }
    }
}


// ==================================================================
// MAIN
// ==================================================================
int main()
{
    printf("=== Productor–Consumidor con OpenMP (locks) ===\n");

    // Inicializar el lock ANTES de entrar a la región paralela
    omp_init_lock(&lockBuffer);

    // Lanzamos 6 hilos: 3 productores (0,2,4) y 3 consumidores (1,3,5)
    #pragma omp parallel num_threads(6)
    {
        int tid = omp_get_thread_num();

        if (tid % 2 == 0)
            productor(tid);   // hilos pares
        else
            consumidor(tid);  // hilos impares
    }

    // Destruir el lock al final
    omp_destroy_lock(&lockBuffer);

    printf("=== FIN: %d producidos / %d consumidos ===\n",
           produced, consumed);

    return 0;
}
