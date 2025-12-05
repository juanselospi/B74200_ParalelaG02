/**
 * Formación correcta de moléculas H2O usando OpenMP.
 *
 * Este problema modela un proceso químico donde:
 *   - Se requieren 2 átomos de Hidrógeno (H)
 *   - y 1 átomo de Oxígeno (O)
 *   - para formar exactamente UNA molécula de agua (H2O).
 *
 * Ningún átomo puede “colarse” solo: deben entrar en grupos correctos.
 *
 * Para garantizarlo usamos "permisos por tipo":
 *   permisosH = número de hidrógenos autorizados a entrar a la molécula
 *   permisosO = número de oxígenos autorizados a entrar a la molécula
 *
 * Cuando el monitor detecta que hay suficientes átomos disponibles:
 *       atomsH >= 2 y atomsO >= 1
 * entonces consume esos átomos de la mesa y libera:
 *       +2 permisosH
 *       +1 permisosO
 *
 * Así cada átomo espera su permiso correspondiente, evitando mezclas incorrectas
 * como: 3H, 1H+2O o entradas desordenadas.
 */

#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <ctime>

#define NUMH 6               // Número total de Hidrógenos disponibles
#define NUMO 3               // Número total de Oxígenos disponibles
#define TOTAL (NUMH + NUMO)  // Total de threads creados

// ---------------------------------------------------------
// Contadores globales compartidos entre todos los threads.
// Estos NO llevan lock explícito; se modifican dentro de
// regiones críticas (#pragma omp critical).
// ---------------------------------------------------------
int atomsH = 0;             // Hidrógenos que han llegado y esperan unión
int atomsO = 0;             // Oxígenos que han llegado y esperan unión

// Permisos necesarios para formar las moléculas
// Estos valores simulan los semáforos individuales para H y O.
int permisosH = 0;          // Permisos disponibles SOLO para H
int permisosO = 0;          // Permisos disponibles SOLO para O


// =========================================================
//  Monitor: decide cuándo puede formarse una molécula H2O
// =========================================================
void intentarFormarMolecula() {

    // IMPORTANTE:
    // Esta función SIEMPRE se llama dentro de un omp critical,
    // por lo que tiene exclusión mutua garantizada.

    // ¿Hay suficientes átomos para una molécula?
    if (atomsH >= 2 && atomsO >= 1) {

        // Consumimos exactamente los átomos requeridos.
        atomsH -= 2;
        atomsO -= 1;

        // Liberamos permisos adecuados:
        //   → 2 hidrógenos
        //   → 1 oxígeno
        permisosH += 2;
        permisosO += 1;

        printf(">> Se formará una nueva molécula H2O\n");

        // -----------------------------------------------
        // 🔧 PUNTO PARA AGREGAR REGLAS EXTRA DEL PROFESOR
        // -----------------------------------------------
        // Por ejemplo:
        // - Llevar conteo global de moléculas formadas
        // - Limitar cuántas moléculas se pueden producir
        // - Introducir prioridad (dar chance a hidrógenos atrasados)
        // - Evitar inanición química
        // - Reglas de “solo formar molécula si hay X condiciones”
        // -----------------------------------------------
    }
}


// =========================================================
//  HIDRÓGENO: comportamiento de un átomo H
// =========================================================
void workerH(int id) {

    // Simulación de tiempos de llegada aleatoria
    usleep(200000 + rand() % 800000);
    printf("H(%d): llegó\n", id);

    // 1) Fase de ARRIBO — entra al monitor a anunciarse
    #pragma omp critical
    {
        atomsH++;                // Se registra como disponible
        intentarFormarMolecula();  // El monitor evalúa si ya se puede formar H2O
    }

    // 2) Espera a que el monitor libere permisos para H
    while (true) {
        bool got = false;

        #pragma omp critical
        {
            if (permisosH > 0) {
                permisosH--;     // Reclama uno de los permisos exclusivos para H
                got = true;
            }
        }

        if (got) break;          // Ya puede formar parte de una molécula

        // Evitar busy waiting agresivo
        usleep(5000);
    }

    // 3) Entra a la molécula
    printf("H(%d): entra a la molécula H2O\n", id);
}


// =========================================================
//  OXÍGENO: comportamiento de un átomo O
// =========================================================
void workerO(int id) {

    usleep(200000 + rand() % 800000);
    printf("O(%d): llegó\n", id);

    // 1) Fase de ARRIBO
    #pragma omp critical
    {
        atomsO++;                // Oxígeno disponible
        intentarFormarMolecula();  // Ver si ya hay 2H + 1O
    }

    // 2) Espera permiso exclusivo para oxígeno
    while (true) {
        bool got = false;

        #pragma omp critical
        {
            if (permisosO > 0) {
                permisosO--;
                got = true;
            }
        }

        if (got) break;

        usleep(5000);
    }

    printf("O(%d): entra a la molécula H2O\n", id);
}


// =========================================================
//  MAIN — crea threads OpenMP y lanza la simulación
// =========================================================
int main() {

    srand(time(NULL));

    printf("=== Formación de moléculas H2O con OpenMP ===\n");

    // Crea TOTAL threads (NUMH Hidrógenos + NUMO Oxígenos)
    #pragma omp parallel num_threads(TOTAL)
    {
        int tid = omp_get_thread_num();

        // Los primeros NUMH threads serán hidrógenos
        if (tid < NUMH)
            workerH(tid);

        // Los siguientes NUMO threads serán oxígenos
        else
            workerO(tid - NUMH);
    }

    printf("=== Fin, todas las moléculas procesadas ===\n");
    return 0;
}
