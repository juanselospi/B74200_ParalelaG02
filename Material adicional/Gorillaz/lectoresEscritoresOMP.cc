/**
 * Lectores–Escritores con PRIORIDAD A ESCRITORES usando OpenMP.
 *
 * Versión "monitorizada":
 *    - Tenemos un "monitor" implementado con:
 *         omp_lock_t monitorLock;
 *         inicioLectura() / finLectura()
 *         inicioEscritura() / finEscritura()
 *
 *    - PRIORIDAD A ESCRITORES:
 *         * Si un escritor quiere entrar, se marca como "esperando".
 *         * Mientras haya al menos un escritor esperando,
 *           NO se permiten nuevos lectores.
 *         * Los lectores que ya estaban dentro pueden terminar.
 *
 * Esta versión es ideal para examen porque:
 *   - Muestra el patrón clásico de Lectores–Escritores.
 *   - Implementa prioridad a escritores sin semáforos POSIX,
 *     solo con OpenMP (locks + variables compartidas).
 */

#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>   // usleep
#include <ctime>

// ---------------------------------------------------------
// Parámetros
// ---------------------------------------------------------
#define NUM_LECTORES   4    // cantidad de hilos lectores
#define NUM_ESCRITORES 2    // cantidad de hilos escritores

#define LECID_BASE     0    // IDs de lectores: 0 .. NUM_LECTORES-1
#define ESCID_BASE     NUM_LECTORES  // IDs de escritores: NUM_LECTORES .. NUM_LECTORES+NUM_ESCRITORES-1

// ---------------------------------------------------------
// Variables compartidas (datos protegidos por el monitor)
// ---------------------------------------------------------
int valorCompartido = 0;          // recurso compartido (ej: una variable o registro)

int lectoresActivos    = 0;       // cuántos lectores están leyendo AHORA
int escritoresEsperando = 0;      // cuántos escritores están esperando entrar
int escritorActivo     = 0;       // 1 si hay escritor escribiendo, 0 si no

// Lock que representa el "monitor"
omp_lock_t monitorLock;

// =========================================================
// FUNCIONES DEL MONITOR
// =========================================================

/**
 * inicioLectura(int idLector)
 *
 *   Lógica para que un lector entre a la sección de lectura.
 *   PRIORIDAD A ESCRITORES:
 *     - Solo permitimos entrar a un lector si:
 *         * NO hay escritorActivo
 *         * NO hay escritoresEsperando
 *
 *   De esa forma, si llega un escritor, se bloquea el paso
 *   de lectores nuevos hasta que ese escritor haya pasado.
 */
void inicioLectura(int idLector) {
    while (true) {

        // Entramos al monitor
        omp_set_lock(&monitorLock);

        // Condición para poder leer:
        //  - no hay escritor dentro
        //  - no hay escritores esperando (prioridad a escritores)
        if (escritorActivo == 0 && escritoresEsperando == 0) {
            lectoresActivos++;
            printf("Lector %d entra a LEER (lectoresActivos=%d, escritoresEsperando=%d)\n",
                   idLector, lectoresActivos, escritoresEsperando);
            // Salimos del monitor y permitimos la lectura
            omp_unset_lock(&monitorLock);
            break;
        }

        // No se pudo entrar: hay escritor escribiendo o escritores esperando.
        // Salimos del monitor y esperamos un rato antes de reintentar.
        omp_unset_lock(&monitorLock);
        usleep(1000); // espera ligera para no quemar CPU
    }
}

/**
 * finLectura(int idLector)
 *
 *   El lector avisa que terminó de leer.
 *   Disminuimos el contador de lectores activos.
 */
void finLectura(int idLector) {
    // Entramos al monitor
    omp_set_lock(&monitorLock);

    lectoresActivos--;
    printf("Lector %d SALE de LEER (lectoresActivos=%d)\n",
           idLector, lectoresActivos);

    // Salimos del monitor
    omp_unset_lock(&monitorLock);
}

/**
 * inicioEscritura(int idEscritor)
 *
 *   Lógica para que un escritor entre a la sección de escritura.
 *
 *   PRIORIDAD A ESCRITORES:
 *     - Apenas el escritor quiere escribir:
 *         escritoresEsperando++
 *
 *     - Luego espera a que:
 *         * NO haya lectores activos
 *         * NO haya escritorActivo
 *     - Cuando entra:
 *         escritoresEsperando--, escritorActivo = 1
 */
void inicioEscritura(int idEscritor) {

    // Primero, anunciar que hay un escritor esperando
    omp_set_lock(&monitorLock);
    escritoresEsperando++;
    printf("Escritor %d QUIERE ESCRIBIR (escritoresEsperando=%d)\n",
           idEscritor, escritoresEsperando);
    omp_unset_lock(&monitorLock);

    while (true) {

        // Entramos al monitor
        omp_set_lock(&monitorLock);

        // Condición para poder escribir:
        //  - no hay nadie escribiendo
        //  - no hay lectores activos
        if (escritorActivo == 0 && lectoresActivos == 0) {
            // Este escritor va a entrar a escribir
            escritoresEsperando--;
            escritorActivo = 1;

            printf("Escritor %d ENTRA a ESCRIBIR (lectoresActivos=%d, escritoresEsperando=%d)\n",
                   idEscritor, lectoresActivos, escritoresEsperando);

            omp_unset_lock(&monitorLock);
            break;
        }

        // No se pudo entrar, soltamos monitor y esperamos
        omp_unset_lock(&monitorLock);
        usleep(1000);
    }
}

/**
 * finEscritura(int idEscritor)
 *
 *   El escritor terminó de escribir. Libera el escritorActivo.
 */
void finEscritura(int idEscritor) {
    omp_set_lock(&monitorLock);

    escritorActivo = 0;
    printf("Escritor %d SALE de ESCRIBIR\n", idEscritor);

    omp_unset_lock(&monitorLock);
}


// =========================================================
// FUNCIONES DE LOS HILOS LECTOR / ESCRITOR
// =========================================================

/**
 * función lectorOMP
 *
 *   Cada lector:
 *     - piensa (simulado con usleep)
 *     - entra al monitor con inicioLectura()
 *     - "lee" valorCompartido
 *     - sale con finLectura()
 *   Esto se repite varias veces.
 */
void lectorOMP(int idLector) {
    // Por ejemplo, cada lector lee 3 veces
    for (int k = 0; k < 3; ++k) {

        // Simulamos que el lector está "pensando" o haciendo otras cosas
        usleep(100000 + rand() % 200000);

        // Entra a leer (usa el monitor)
        inicioLectura(idLector);

        // Sección de lectura (NO modificamos valorCompartido)
        printf("   >> Lector %d LEYENDO valorCompartido = %d\n",
               idLector, valorCompartido);

        // Simula tiempo leyendo
        usleep(150000);

        // Sale de leer
        finLectura(idLector);
    }
}

/**
 * función escritorOMP
 *
 *   Cada escritor:
 *     - piensa un rato
 *     - pide entrar con inicioEscritura()
 *     - modifica valorCompartido
 *     - sale con finEscritura()
 *   Esto se repite varias veces.
 */
void escritorOMP(int idEscritor) {
    // Por ejemplo, cada escritor escribe 2 veces
    for (int k = 0; k < 2; ++k) {

        // Simulamos tiempo "pensando" o trabajando fuera de la sección crítica
        usleep(200000 + rand() % 200000);

        // Entrar a escribir
        inicioEscritura(idEscritor);

        // Sección de escritura: modificamos el recurso compartido
        int valorAnterior = valorCompartido;
        valorCompartido = valorAnterior + 10;  // ejemplo: sumar 10

        printf("   >> Escritor %d ESCRIBE valorCompartido: %d -> %d\n",
               idEscritor, valorAnterior, valorCompartido);

        // Simulamos tiempo escribiendo
        usleep(150000);

        // Salimos de la escritura
        finEscritura(idEscritor);
    }
}


// =========================================================
// MAIN
// =========================================================
int main() {
    srand(time(NULL));

    printf("=== Lectores–Escritores con prioridad a ESCRITORES (OpenMP) ===\n");

    // Inicializamos el lock del monitor
    omp_init_lock(&monitorLock);

    // Total de threads = lectores + escritores
    int totalThreads = NUM_LECTORES + NUM_ESCRITORES;

    #pragma omp parallel num_threads(totalThreads)
    {
        int tid = omp_get_thread_num();

        if (tid < NUM_LECTORES) {
            // Estos hilos serán lectores
            int idLector = tid;  // IDs 0..NUM_LECTORES-1
            lectorOMP(idLector);
        } else {
            // Estos hilos serán escritores
            int idEscritor = tid - NUM_LECTORES;  // IDs 0..NUM_ESCRITORES-1
            escritorOMP(idEscritor);
        }
    }

    // Destruimos el lock
    omp_destroy_lock(&monitorLock);

    printf("=== FIN. valorCompartido final = %d ===\n", valorCompartido);
    return 0;
}
