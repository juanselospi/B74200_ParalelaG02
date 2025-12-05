/**
 * Formación correcta de moléculas H2O usando MPI.
 *
 * Este programa modela la reacción química:
 *      2 Hidrógenos (H) + 1 Oxígeno (O) → 1 molécula de agua (H2O)
 *
 * El proceso 0 funciona como un *monitor centralizado* (igual a un monitor
 * Mesa en OpenMP):
 *     - Recibe mensajes de llegada de átomos
 *     - Lleva los contadores atomsH y atomsO
 *     - Cuando detecta que hay 2H y 1O → "forma" una molécula
 *     - Libera exactamente 2 permisosH y 1 permisoO
 *
 * Cada átomo (otros procesos MPI) se comporta así:
 *     1) Se anuncia al monitor
 *     2) Espera un permiso (con timeout)
 *     3) Si recibe permiso → entra a una molécula
 *     4) Si vence el timeout → se descarta (no será usado)
 *     5) En ambos casos avisa al monitor que terminó (mensaje 999)
 *
 * Esto evita deadlocks cuando sobran átomos.
 */

#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <ctime>

// -------------------------------------------------------------
// CANTIDADES DE ÁTOMOS PARA ESTA EJECUCIÓN
// -------------------------------------------------------------
#define NUMH 4              // 4 Hidrógenos
#define NUMO 3              // 3 Oxígenos
#define TOTAL (NUMH + NUMO) // TOTAL de procesos que son átomos

#define MONITOR 0           // El proceso 0 coordina todo

// -------------------------------------------------------------
// TAGS para enviar mensajes al monitor
// -------------------------------------------------------------
#define TAG_LLEGO_H    1    // Un H llegó
#define TAG_LLEGO_O    2    // Un O llegó
#define TAG_PERMISO_H  3    // El monitor deja entrar un H
#define TAG_PERMISO_O  4    // El monitor deja entrar un O

// ============================================================================
// MONITOR (proceso 0)
// ============================================================================
// Este proceso:
//   - Mantiene los contadores atomsH y atomsO
//   - Libera permisos para formar moléculas exactas
//   - Cuenta cuantos átomos terminaron
//   - Finaliza cuando TODOS los átomos hayan terminado, entren o no.
//
// IMPORTANTE: este monitor NO hace timeout; solo reacciona
// al tráfico de mensajes de los átomos.
// ============================================================================
void monitorMPI() {

    int atomsH = 0, atomsO = 0;   // Contadores de llegada de H y O
    int permisosH = 0, permisosO = 0; // Permisos acumulados
    int terminados = 0;           // Átomos que ya terminaron su trabajo

    MPI_Status st;

    // Esperar hasta que TODOS los átomos terminen (entren o sean descartados)
    while (terminados < TOTAL) {
        int dummy;

        // Recibir cualquier mensaje de cualquier átomo
        MPI_Recv(&dummy, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                 MPI_COMM_WORLD, &st);

        int id  = st.MPI_SOURCE;  // Qué proceso envió el mensaje
        int tag = st.MPI_TAG;     // Qué tipo de mensaje es


        // ---------------------------------------------------
        // LLEGÓ UN HIDRÓGENO
        // ---------------------------------------------------
        if (tag == TAG_LLEGO_H) {

            atomsH++; // Registrar llegada del H

            // Si hay suficientes átomos → formar molécula
            if (atomsH >= 2 && atomsO >= 1) {
                atomsH -= 2;
                atomsO -= 1;

                permisosH += 2;
                permisosO += 1;

                printf(">> MONITOR: Se formará una molécula H2O\n");
            }

            // Si hay permisos para H disponibles, darlos inmediatamente
            if (permisosH > 0) {
                permisosH--;
                MPI_Send(&dummy, 1, MPI_INT, id,
                         TAG_PERMISO_H, MPI_COMM_WORLD);
            }
        }


        // ---------------------------------------------------
        // LLEGÓ UN OXÍGENO
        // ---------------------------------------------------
        else if (tag == TAG_LLEGO_O) {

            atomsO++;

            if (atomsH >= 2 && atomsO >= 1) {
                atomsH -= 2;
                atomsO -= 1;

                permisosH += 2;
                permisosO += 1;

                printf(">> MONITOR: Se formará una molécula H2O\n");
            }

            if (permisosO > 0) {
                permisosO--;
                MPI_Send(&dummy, 1, MPI_INT, id,
                         TAG_PERMISO_O, MPI_COMM_WORLD);
            }
        }


        // ---------------------------------------------------
        // ÁTOMO TERMINÓ (sea usado o descartado)
        // ---------------------------------------------------
        else {
            terminados++;

            // ------------------------------------------------------------------
            // 🔧 PUNTO PARA AGREGAR REGLAS SI EL PROFESOR LO PIDE:
            //  - Llevar conteo de cuántas moléculas van formadas
            //  - Detectar si se llegó a un número máximo de moléculas
            //  - Implementar fairness: evitar que ciertos átomos esperen mucho
            //  - Imprimir diagnóstico completo del estado químico
            // ------------------------------------------------------------------
        }
    }

    printf("Monitor: todas las moléculas procesadas.\n");
}


// ============================================================================
// HIDRÓGENO
// ============================================================================
void workerH(int id) {
    int dummy = 1;

    // Simular tiempos de llegada aleatorios
    usleep(200000 + rand() % 800000);
    printf("H(%d): llegó\n", id);

    // Notificar al monitor que llegó un H
    MPI_Send(&dummy, 1, MPI_INT, MONITOR, TAG_LLEGO_H, MPI_COMM_WORLD);

    // ======================================================
    // ESPERAR PERMISO CON TIMEOUT
    // ======================================================
    MPI_Status st;
    int flag = 0;
    int intentos = 0;
    const int MAX_INTENTOS = 2000; // Tiempo máximo esperando permiso (~2 s)

    // Intentar recibir permiso sin bloquear usando MPI_Iprobe
    while (!flag && intentos < MAX_INTENTOS) {

        // Iprobe → pregunta si hay un mensaje sin bloquear
        MPI_Iprobe(MONITOR, TAG_PERMISO_H, MPI_COMM_WORLD, &flag, &st);

        if (!flag) {
            usleep(1000); // Evita busy-wait
            intentos++;
        }
    }

    // ========================== RECIBIÓ PERMISO ✨ ==========================
    if (flag) {
        MPI_Recv(&dummy, 1, MPI_INT, MONITOR, TAG_PERMISO_H,
                 MPI_COMM_WORLD, &st);

        printf("H(%d): entra a una molécula H2O\n", id);

        // Avisar que terminó correctamente
        MPI_Send(&dummy, 1, MPI_INT, MONITOR, 999, MPI_COMM_WORLD);
        return;
    }

    // ======================== TIMEOUT → DESCARTADO ========================
    printf("H(%d): no será usado (átomo descartado)\n", id);

    // Avisar al monitor que este átomo ya no participará
    MPI_Send(&dummy, 1, MPI_INT, MONITOR, 999, MPI_COMM_WORLD);
}



// ============================================================================
// OXÍGENO
// ============================================================================
// Exactamente igual que H, pero con TAGs para oxígeno.
void workerO(int id) {
    int dummy = 1;
    usleep(200000 + rand() % 800000);

    printf("O(%d): llegó\n", id);

    MPI_Send(&dummy, 1, MPI_INT, MONITOR, TAG_LLEGO_O, MPI_COMM_WORLD);

    MPI_Status st;
    int flag = 0;
    int intentos = 0;
    const int MAX_INTENTOS = 2000;

    while (!flag && intentos < MAX_INTENTOS) {
        MPI_Iprobe(MONITOR, TAG_PERMISO_O, MPI_COMM_WORLD, &flag, &st);

        if (!flag) {
            usleep(1000);
            intentos++;
        }
    }

    if (flag) {
        MPI_Recv(&dummy, 1, MPI_INT, MONITOR, TAG_PERMISO_O,
                 MPI_COMM_WORLD, &st);

        printf("O(%d): entra a una molécula H2O\n", id);

        MPI_Send(&dummy, 1, MPI_INT, MONITOR, 999, MPI_COMM_WORLD);
        return;
    }

    printf("O(%d): no será usado (átomo descartado)\n", id);

    MPI_Send(&dummy, 1, MPI_INT, MONITOR, 999, MPI_COMM_WORLD);
}



// ============================================================================
// MAIN
// ============================================================================
int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int id, nproc;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);    // ID del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &nproc); // Total de procesos lanzados

    // Verificar que haya suficientes procesos MPI
    if (nproc < TOTAL + 1) { // +1 es el monitor
        if (id == 0)
            printf("ERROR: se necesitan %d procesos (1 monitor + %d átomos)\n",
                   TOTAL + 1, TOTAL);
        MPI_Finalize();
        return 0;
    }

    srand(time(NULL) + id);

    // Lanzar el rol correspondiente a cada proceso
    if (id == MONITOR)
        monitorMPI();
    else if (id <= NUMH)
        workerH(id);         // Procesos 1..NUMH son H
    else
        workerO(id - NUMH);  // Procesos NUMH+1..NUMH+NUMO son O

    MPI_Finalize();
    return 0;
}
