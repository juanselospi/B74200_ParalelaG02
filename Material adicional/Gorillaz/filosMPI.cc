// filosofosMPI.cc
// ------------------------------------------------------------
// Versión correcta del problema de los filósofos usando MPI.
// Emula el comportamiento de un MONITOR (Mesa) como en OpenMP:
//   - Estados PENSANDO, HAMBRIENTO, COMIENDO
//   - Función probar(i) que decide si un filósofo puede comer
//   - Cuando uno termina, se revisan los vecinos inmediatamente
//
// Garantía: dos filósofos VECINOS nunca comen al mismo tiempo.
// ------------------------------------------------------------

#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#define NUM_FILOSOFOS   5      // Cantidad total de filósofos
#define VECES_MAX_COMER 3      // Cuántas veces comerá cada filósofo

// ----------------------
// Tipos de mensaje MPI
// ----------------------
#define TAG_HAMBRIENTO  1      // Filósofo → Monitor: "quiero comer"
#define TAG_TERMINO     2      // Filósofo → Monitor: "ya terminé de comer"
#define TAG_PERMISO     3      // Monitor → Filósofo: "puedes comer"

// -------------------------------------------------------------
// Estados posibles (igual que en OpenMP con monitor Mesa)
// -------------------------------------------------------------
enum Estado {
    PENSANDO = 0,
    HAMBRIENTO,
    COMIENDO
};

// -------------------------------------------------------------
// Funciones para obtener vecinos de un filósofo (1..NUM_FILOSOFOS)
// Nota: usamos índices 1 a N para evitar confusiones con el rank 0.
// -------------------------------------------------------------
int izq(int i) { return (i == 1) ? NUM_FILOSOFOS : i - 1; }
int der(int i) { return (i == NUM_FILOSOFOS) ? 1 : i + 1; }

// ==========================================================================
//  FUNCIÓN probar()
// --------------------------------------------------------------------------
//  Equivalente directo al método probar(i) de la solución OpenMP.
//
//  Revisa si el filósofo i puede pasar de HAMBRIENTO → COMIENDO.
//  Condición: sus dos vecinos NO deben estar COMIENDO.
//
//  Si puede comer:
//     - Se actualiza estado[i] = COMIENDO
//     - Se envía inmediatamente un mensaje TAG_PERMISO a MPI rank = i
//
//  IMPORTANTE: 
//  Este es el LUGAR PERFECTO para agregar condiciones nuevas en un examen.
//  EJEMPLO: evitar inanición, darle prioridad a un filósofo, etc.
// ==========================================================================
void probar(int i, Estado estado[], MPI_Comm comm)
{
    // Condición clásica de filósofos comensales
    if (estado[i] == HAMBRIENTO &&
        estado[izq(i)] != COMIENDO &&
        estado[der(i)] != COMIENDO)
    {
        // Puede comer → cambiamos estado
        estado[i] = COMIENDO;

        // Enviamos el permiso al filósofo i
        int permiso = 1;  // solo un valor dummy para indicar "puedes comer"
        MPI_Send(&permiso, 1, MPI_INT, i, TAG_PERMISO, comm);
    }

    // Si NO puede comer, no enviamos nada.
    // El filósofo volverá a solicitar más adelante.
}

// ==========================================================================
//  PROCESO 0: MONITOR
// --------------------------------------------------------------------------
//  Controla todo el sistema.
//
//  Responsabilidades:
//    ● Mantiene estados de cada filósofo.
//    ● Recibe solicitudes de comer.
//    ● Ejecuta probar(i) para decidir si puede comer.
//    ● Cuando un filósofo termina, revisa a los vecinos con probar(vecino).
//    ● Evita que vecinos coman al mismo tiempo.
//
//  Este proceso es EXACTAMENTE equivalente al monitor Mesa.
// ==========================================================================
void monitorMPI()
{
    Estado estado[NUM_FILOSOFOS + 1];      // estados 1..N
    int vecesComio[NUM_FILOSOFOS + 1];     // para llevar control de progreso

    // Inicializamos todos pensando
    for (int i = 1; i <= NUM_FILOSOFOS; ++i) {
        estado[i] = PENSANDO;
        vecesComio[i] = 0;
    }

    int terminados = 0;     // cuántos completaron sus comidas
    MPI_Status st;

    // Loop principal del monitor → corre hasta que TODOS terminan
    while (terminados < NUM_FILOSOFOS) {

        int i;   // aquí recibimos el ID del filósofo que envía
        MPI_Recv(&i, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                 MPI_COMM_WORLD, &st);

        int tag = st.MPI_TAG;

        // ------------------------------------------------------------------
        // Caso 1: Filósofo i pide comer
        // ------------------------------------------------------------------
        if (tag == TAG_HAMBRIENTO) {
            estado[i] = HAMBRIENTO;

            // Intentamos ver si ya puede comer
            probar(i, estado, MPI_COMM_WORLD);

            // NOTA: si no puede, NO enviamos nada. El filósofo volverá a pedir.
        }

        // ------------------------------------------------------------------
        // Caso 2: Filósofo i terminó de comer
        // ------------------------------------------------------------------
        else if (tag == TAG_TERMINO) {

            estado[i] = PENSANDO;
            vecesComio[i]++;

            // Contabilizamos si ya completó su ciclo
            if (vecesComio[i] == VECES_MAX_COMER)
                terminados++;

            // Al soltar los tenedores, revisamos a los vecinos:
            // Esto es equivalente al monitor OpenMP (soltar → probar a vecinos).
            probar(izq(i), estado, MPI_COMM_WORLD);
            probar(der(i), estado, MPI_COMM_WORLD);

            // NOTA: Este punto también es IDEAL para agregar reglas nuevas.
            //
            // EJEMPLOS:
            //  - Prohibir que un filósofo coma dos veces seguidas sin descanso.
            //  - Asegurar que todos coman la misma cantidad de veces.
            //  - Implementar prioridades personalizadas.
        }
    }

    printf("Monitor: todos los filósofos terminaron sus comidas.\n");
}

// ==========================================================================
//  PROCESO FILÓSOFO (ranks 1..NUM_FILOSOFOS)
// --------------------------------------------------------------------------
//  Ciclo clásico:
//    1. Pensar
//    2. Pedir permiso para comer
//    3. Esperar TAG_PERMISO del monitor
//    4. Comer
//    5. Avisar que terminó
// ==========================================================================
void filosofoMPI(int id)
{
    srand(time(NULL) + id);  // semilla distinta para cada proceso
    MPI_Status st;
    int permiso;

    for (int k = 0; k < VECES_MAX_COMER; ++k) {

        // -----------------------------
        // 1. Pensar (simulación simple)
        // -----------------------------
        for (volatile long w = 0; w < 5000000; ++w) {}
        printf("Filosofo %d está pensando.\n", id);

        // -----------------------------
        // 2. Pedir comer al monitor
        // -----------------------------
        MPI_Send(&id, 1, MPI_INT, 0, TAG_HAMBRIENTO, MPI_COMM_WORLD);

        // -----------------------------
        // 3. Esperar permiso
        //    bloquear hasta que el monitor envíe TAG_PERMISO
        // -----------------------------
        MPI_Recv(&permiso, 1, MPI_INT, 0, TAG_PERMISO,
                 MPI_COMM_WORLD, &st);

        printf("Filosofo %d está COMIENDO.\n", id);

        // -----------------------------
        // 4. Comer (trabajo simulado)
        // -----------------------------
        for (volatile long w = 0; w < 5000000; ++w) {}

        // -----------------------------
        // 5. Avisar al monitor que terminó
        // -----------------------------
        MPI_Send(&id, 1, MPI_INT, 0, TAG_TERMINO, MPI_COMM_WORLD);
        printf("Filosofo %d terminó una comida.\n", id);
    }

    printf("Filosofo %d terminó sus %d comidas.\n", id, VECES_MAX_COMER);
}

// ==========================================================================
//  MAIN
// --------------------------------------------------------------------------
//  rank 0 → monitor
//  ranks 1..N → filósofos
// ==========================================================================
int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int id, n;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &n);

    // Necesitamos 1 monitor + N filósofos
    if (n < NUM_FILOSOFOS + 1) {
        if (id == 0)
            printf("ERROR: se necesitan al menos %d procesos.\n",
                   NUM_FILOSOFOS + 1);
        MPI_Finalize();
        return 0;
    }

    if (id == 0)
        monitorMPI();          // Proceso 0 → monitor
    else if (id <= NUM_FILOSOFOS)
        filosofoMPI(id);       // Procesos 1..N → filósofos

    MPI_Finalize();
    return 0;
}
