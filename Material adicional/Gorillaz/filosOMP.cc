// filosofosOMP.cc
// Versión del problema de los filósofos comensales usando OpenMP
// Simula la idea de un monitor (Mesa) con estados y una función probar()
// NO usa semáforos ni pthreads, solo OpenMP y variables compartidas.

#include <cstdio>      // printf
#include <cstdlib>     // rand, srand
#include <ctime>       // time para la semilla de rand
#include <omp.h>       // OpenMP

// ---------------------------------------------------------------------
// Parámetros del problema
// ---------------------------------------------------------------------
const int NUM_FILOSOFOS   = 5;  // cantidad de filósofos (y tenedores)
const int VECES_MAX_COMER = 3;  // cuántas veces come cada filósofo antes de terminar

// ---------------------------------------------------------------------
// Estados posibles de cada filósofo (esto imita lo que hacías con Mesa)
// ---------------------------------------------------------------------
// Aquí podrías agregar más estados si el profe te lo pide, por ejemplo:
// MEDITANDO, ESPERANDO_POSTRE, etc.
enum Estado {
    PENSANDO = 0,
    HAMBRIENTO,
    COMIENDO
};

// Arreglo global de estados, uno por filósofo
// Se comparte entre todos los hilos (filósofos)
Estado estado[NUM_FILOSOFOS];

// (Opcional) Contador de cuántas veces ha comido cada filósofo
// Esto es muy útil si el profe pide condiciones tipo “justicia” o “no inanición”.
int vecesComio[NUM_FILOSOFOS];

// ---------------------------------------------------------------------
// Funciones auxiliares para obtener el vecino izquierdo y derecho
// ---------------------------------------------------------------------
int vecino_izquierdo(int i) {
    // El filósofo izquierdo es (i - 1) con índice circular
    return (i + NUM_FILOSOFOS - 1) % NUM_FILOSOFOS;
}

int vecino_derecho(int i) {
    // El filósofo derecho es (i + 1) con índice circular
    return (i + 1) % NUM_FILOSOFOS;
}

// ---------------------------------------------------------------------
// Simulaciones "tontas" de pensar y comer
// Aquí podrías cambiar el "trabajo" según lo que te pidan.
// ---------------------------------------------------------------------
void pensar(int id) {
    // Simula que el filósofo está pensando
    // En un examen, no importa el contenido, solo la idea de que hace algo.
    // Usamos un pequeño ciclo ocupado (busy-wait) para no depender de sleep().
    for (volatile long i = 0; i < 1000000; ++i) {
        // no hace nada, solo consume tiempo
    }

    // Imprimimos fuera de la sección crítica del "monitor" pero
    // protegemos el printf con un critical separado para que no se mezclen las líneas.
    #pragma omp critical (IO)
    {
        printf("Filosofo %d está pensando.\n", id);
    }
}

void comer(int id) {
    // Simula que el filósofo está comiendo
    for (volatile long i = 0; i < 1000000; ++i) {
        // no hace nada, solo consume tiempo
    }

    #pragma omp critical (IO)
    {
        printf("Filosofo %d está COMIENDO.\n", id);
    }
}

// ---------------------------------------------------------------------
// Función probar(i)
// Equivalente a la función "test" del monitor Mesa.
// Revisa si el filósofo i puede pasar de HAMBRIENTO a COMIENDO.
// ---------------------------------------------------------------------
// MUY IMPORTANTE: esta función se supone que se llama SIEMPRE dentro de
// la región crítica del "monitor" (#pragma omp critical (MONITOR)).
void probar(int i) {
    int izq = vecino_izquierdo(i);
    int der = vecino_derecho(i);

    // Si yo estoy hambriento y mis vecinos NO están comiendo,
    // entonces puedo pasar a COMIENDO.
    if (estado[i] == HAMBRIENTO &&
        estado[izq] != COMIENDO &&
        estado[der] != COMIENDO) {

        estado[i] = COMIENDO;

        // Aquí sería el equivalente a hacer signal(&cond[i]) en un monitor real.
        // En OpenMP no tenemos condition variables, así que el hilo que está
        // esperando va a revisar periódicamente el estado[i].
        // Si el profe te pide condiciones extra, normalmente se agregan AQUÍ:
        //  - que cierto filósofo tenga prioridad
        //  - que no coma dos veces seguidas
        //  - que no haya inanición, etc.
    }
}

// ---------------------------------------------------------------------
// Función tomarTenedores(i)
// El filósofo i se declara HAMBRIENTO y pide permiso para comer.
// Equivalente a "pickup(i)" / "tomar(i)" en la solución con monitor.
// ---------------------------------------------------------------------
void tomarTenedores(int i) {
    bool tengoPermiso = false;

    // Bucle de espera hasta que el filósofo logre cambiar su estado a COMIENDO
    // usando la función probar() dentro de la región crítica.
    while (!tengoPermiso) {

        // Región crítica que simula la entrada al monitor Mesa.
        #pragma omp critical (MONITOR)
        {
            // Paso a estado HAMBRIENTO (si no lo estaba ya)
            estado[i] = HAMBRIENTO;

            // Intento ver si puedo comer.
            probar(i);

            // Si después de probar quedé en COMIENDO, ya tengo permiso.
            if (estado[i] == COMIENDO) {
                tengoPermiso = true;
            }
        } // fin de la región crítica MONITOR

        // Si todavía no tengo permiso, vuelvo al while y lo intento de nuevo.
        // En un monitor real aquí harías un "wait(cond[i])".
        // Aquí lo estamos simulando con un pequeño bucle de reintento (busy-wait).
        // Podrías agregar un pequeño trabajo aquí para no saturar:
        // for (volatile int k = 0; k < 10000; ++k) {}
    }

    // Cuando salimos del while, ya tenemos "tenedores" lógicamente,
    // porque estamos en estado COMIENDO.
    #pragma omp critical (IO)
    {
        printf("Filosofo %d logró tomar los tenedores (entra a COMER).\n", i);
    }
}

// ---------------------------------------------------------------------
// Función soltarTenedores(i)
// El filósofo i suelta los tenedores y vuelve a PENSANDO.
// Además, intenta despertar (dar paso) a sus vecinos.
// Equivalente a "putdown(i)" / "soltar(i)" en el monitor.
// ---------------------------------------------------------------------
void soltarTenedores(int i) {
    #pragma omp critical (MONITOR)
    {
        // Paso a estado PENSANDO: ya no uso los tenedores.
        estado[i] = PENSANDO;

        // Aquí podría aumentar el contador de cuántas veces he comido.
        // Esto es útil si el profe te pide restricciones basadas en conteos.
        vecesComio[i]++;

        // Intento despertar (permitir comer) al vecino izquierdo y derecho.
        // Si alguno está HAMBRIENTO y el otro vecino también lo permite,
        // probar() lo cambiará a COMIENDO.
        probar(vecino_izquierdo(i));
        probar(vecino_derecho(i));
    }

    #pragma omp critical (IO)
    {
        printf("Filosofo %d soltó los tenedores y vuelve a PENSAR.\n", i);
    }
}

// ---------------------------------------------------------------------
// Función principal
// Crea un hilo OpenMP por filósofo y corre el ciclo pensar-comer varias veces.
// ---------------------------------------------------------------------
int main() {
    // Semilla para rand() si quisieras meter algo aleatorio
    srand(time(nullptr));

    // Inicializamos los estados y contadores
    for (int i = 0; i < NUM_FILOSOFOS; ++i) {
        estado[i]     = PENSANDO;
        vecesComio[i] = 0;
    }

    // -----------------------------------------------------------------
    // Zona paralela: cada hilo representa un filósofo.
    // num_threads(NUM_FILOSOFOS) asegura uno por filósofo.
    // -----------------------------------------------------------------
    #pragma omp parallel num_threads(NUM_FILOSOFOS) shared(estado, vecesComio)
    {
        // id del filósofo = id del hilo
        int id = omp_get_thread_num();

        // Cada filósofo va a pensar y comer varias veces.
        for (int k = 0; k < VECES_MAX_COMER; ++k) {

            // 1) El filósofo piensa
            pensar(id);

            // 2) Intenta tomar los tenedores (entra al "monitor")
            tomarTenedores(id);

            // 3) Come
            comer(id);

            // 4) Suelta los tenedores y da chance a sus vecinos
            soltarTenedores(id);
        }

        // Mensaje final de cada filósofo cuando terminó sus comidas
        #pragma omp critical (IO)
        {
            printf("Filosofo %d terminó de comer %d veces.\n", id, vecesComio[id]);
        }
    } // fin de la región paralela

    // Mensaje final del programa
    printf("Todos los filósofos terminaron. Fin del programa.\n");

    return 0;
}
