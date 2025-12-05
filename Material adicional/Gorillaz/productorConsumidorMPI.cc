/**
 * Problema Productor–Consumidor usando MPI
 *
 * Diseño:
 *   - Proceso 0: MONITOR central con buffer circular verdadero.
 *   - Procesos 1..NPROD: productores.
 *   - Procesos NPROD+1..NPROD+NCONS: consumidores.
 *
 * El monitor:
 *   - Lleva buffer[], inPos, outPos, count.
 *   - Lleva contadores produced y consumed.
 *   - Cuando ya se consumieron TOTAL_ITEMS, empieza a mandar TAG_FIN
 *     a todos los procesos para que terminen.
 *
 * Cada productor/consumidor:
 *   - Entra en un ciclo:
 *        (1) Envía una petición al monitor.
 *        (2) Recibe respuesta.
 *        (3) Actúa según el TAG recibido (OK, RETRY, FIN).
 */

#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>   // usleep

// Parámetros del problema
#define BUFFER_SIZE   5     // tamaño del buffer circular
#define TOTAL_ITEMS  20     // total global de ítems a PRODUCIR/CONSUMIR

// Cantidad de productores y consumidores
#define NPROD 3
#define NCONS 3

// Tags de mensajes
#define TAG_PRODUCE_REQ   1   // productor -> monitor: "quiero producir"
#define TAG_PRODUCE_ITEM  2   // productor -> monitor: envía el dato real
#define TAG_CONSUME_REQ   3   // consumidor -> monitor: "quiero consumir"
#define TAG_CONSUME_ITEM  4   // monitor  -> consumidor: envía el dato real
#define TAG_RETRY         5   // monitor  -> worker: "intente más tarde"
#define TAG_FIN           6   // monitor  -> worker: "no hay más trabajo, termine"


// ======================================================================
// FUNCIÓN: monitorMPI
//   Proceso 0: controla el buffer, acepta producciones y consumos.
// ======================================================================
void monitorMPI(int nproc)
{
    // Buffer circular
    int buffer[BUFFER_SIZE];
    int inPos = 0;    // próxima posición de escritura
    int outPos = 0;   // próxima posición de lectura
    int count = 0;    // cuántos elementos hay en el buffer

    // Contadores globales de items
    int produced = 0; // cuántos items se han producido (aceptados en buffer)
    int consumed = 0; // cuántos items se han entregado a consumidores

    int nWorkers = nproc - 1;  // todos menos el monitor
    bool finished[64];         // marca qué procesos ya recibieron TAG_FIN
    for (int i = 0; i < 64; ++i) finished[i] = false;

    int finishedCount = 0;     // cuántos workers ya terminaron
    bool done = false;         // indica si ya se alcanzó TOTAL_ITEMS consumidos

    MPI_Status st;
    int msg;

    printf("Monitor: iniciado.\n");

    // Bucle principal del monitor:
    // Continúa hasta que TODOS los procesos (productores y consumidores)
    // hayan recibido un TAG_FIN.
    while (finishedCount < nWorkers) {

        // Esperamos cualquier mensaje de cualquier proceso, de cualquier TAG.
        MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                 MPI_COMM_WORLD, &st);

        int from = st.MPI_SOURCE; // rank del que envía
        int tag  = st.MPI_TAG;    // tipo de mensaje

        // Si este worker ya estaba marcado como terminado, ignoramos lo que mande.
        if (finished[from]) {
            continue;
        }

        // ---------------------------------------------------------
        // Si ya se consumieron todos los items y el buffer está vacío,
        // el monitor entra en modo "solo repartir FIN".
        // ---------------------------------------------------------
        if (done) {
            // A cualquier mensaje que llegue, respondemos FIN y marcamos.
            MPI_Send(&msg, 1, MPI_INT, from, TAG_FIN, MPI_COMM_WORLD);
            finished[from] = true;
            finishedCount++;
            continue;
        }

        // ---------------------------------------------------------
        // Llega un PRODUCTOR pidiendo permiso para PRODUCIR.
        // ---------------------------------------------------------
        if (tag == TAG_PRODUCE_REQ) {

            // Si ya no queremos más ítems (produced >= TOTAL_ITEMS),
            // entonces este productor ya no es necesario ⇒ FIN.
            if (produced >= TOTAL_ITEMS) {
                MPI_Send(&msg, 1, MPI_INT, from, TAG_FIN, MPI_COMM_WORLD);
                finished[from] = true;
                finishedCount++;
            }
            else if (count < BUFFER_SIZE) {
                // Hay espacio en el buffer ⇒ aceptar producción.

                // Primero, le enviamos un OK al productor
                MPI_Send(&msg, 1, MPI_INT, from, TAG_PRODUCE_ITEM, MPI_COMM_WORLD);

                // Luego, recibimos el item real desde ese productor.
                int item;
                MPI_Recv(&item, 1, MPI_INT, from, TAG_PRODUCE_ITEM,
                         MPI_COMM_WORLD, &st);

                // Guardamos el item en el buffer circular
                buffer[inPos] = item;
                inPos = (inPos + 1) % BUFFER_SIZE;
                count++;
                produced++;

                printf("[MONITOR] Guardado %d (count=%d, produced=%d)\n",
                       item, count, produced);
            }
            else {
                // Buffer lleno pero aún no llegamos a TOTAL_ITEMS ⇒ RETRY.
                MPI_Send(&msg, 1, MPI_INT, from, TAG_RETRY, MPI_COMM_WORLD);
            }
        }

        // ---------------------------------------------------------
        // Llega un CONSUMIDOR pidiendo un item para consumir.
        // ---------------------------------------------------------
        else if (tag == TAG_CONSUME_REQ) {

            // Si ya se consumieron todos los items y no hay nada en buffer,
            // entonces este consumidor no tiene más trabajo.
            if (consumed >= TOTAL_ITEMS && count == 0) {
                MPI_Send(&msg, 1, MPI_INT, from, TAG_FIN, MPI_COMM_WORLD);
                finished[from] = true;
                finishedCount++;
            }
            else if (count > 0) {
                // Hay algo para consumir en el buffer.

                int item = buffer[outPos];
                outPos = (outPos + 1) % BUFFER_SIZE;
                count--;
                consumed++;

                // Le enviamos el item al consumidor
                MPI_Send(&item, 1, MPI_INT, from, TAG_CONSUME_ITEM, MPI_COMM_WORLD);

                printf("[MONITOR] Entregado %d (count=%d, consumed=%d)\n",
                       item, count, consumed);

                // Si ya consumimos todos los items y además el buffer está vacío,
                // marcamos done = true. Lo que llegue después será FIN.
                if (consumed >= TOTAL_ITEMS && count == 0) {
                    done = true;
                }
            }
            else {
                // Buffer vacío pero aún no se ha completado el TOTAL_ITEMS:
                // el consumidor debe reintentar más tarde.
                int dummy = -1;
                MPI_Send(&dummy, 1, MPI_INT, from, TAG_RETRY, MPI_COMM_WORLD);
            }
        }
        // Cualquier otro TAG lo ignoramos, pero en este diseño no se usan más.
    }

    printf("Monitor: completado. Total consumidos = %d\n", consumed);
}


// ======================================================================
// FUNCIÓN: productorMPI
//   Cada productor envía TAG_PRODUCE_REQ y espera respuesta.
//   - Si recibe TAG_PRODUCE_ITEM: envía el valor y sigue.
//   - Si recibe TAG_RETRY: espera un poco y reintenta.
//   - Si recibe TAG_FIN: termina.
// ======================================================================
void productorMPI(int id)
{
    int msg = 1;          // mensaje dummy (no nos importa el valor)
    MPI_Status st;
    int item = id * 100;  // cada productor arranca con base distinta

    while (true) {

        // Pedimos permiso al monitor para producir.
        MPI_Send(&msg, 1, MPI_INT, 0, TAG_PRODUCE_REQ, MPI_COMM_WORLD);

        // Esperamos respuesta del monitor (OK, RETRY o FIN).
        int resp;
        MPI_Recv(&resp, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &st);

        if (st.MPI_TAG == TAG_FIN) {
            // El monitor indica que ya no necesita más items de este productor.
            printf("Productor %d: FIN recibido, termino.\n", id);
            break;
        }
        else if (st.MPI_TAG == TAG_RETRY) {
            // El monitor dice: buffer lleno, inténtelo de nuevo más tarde.
            usleep(5000); // pequeña pausa
            continue;
        }
        else if (st.MPI_TAG == TAG_PRODUCE_ITEM) {
            // El monitor aceptó producción, enviamos el item real.
            MPI_Send(&item, 1, MPI_INT, 0, TAG_PRODUCE_ITEM, MPI_COMM_WORLD);
            printf("Productor %d produjo %d\n", id, item);
            item++;          // siguiente valor
            usleep(200000);  // simulamos trabajo de producción
        }
    }
}


// ======================================================================
// FUNCIÓN: consumidorMPI
//   Cada consumidor envía TAG_CONSUME_REQ y espera respuesta.
//   - Si recibe TAG_CONSUME_ITEM: procesa el valor y sigue.
//   - Si recibe TAG_RETRY: espera y reintenta.
//   - Si recibe TAG_FIN: termina.
// ======================================================================
void consumidorMPI(int id)
{
    int msg = 1;
    MPI_Status st;

    while (true) {

        // Pedimos un item al monitor.
        MPI_Send(&msg, 1, MPI_INT, 0, TAG_CONSUME_REQ, MPI_COMM_WORLD);

        int item;
        MPI_Recv(&item, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &st);

        if (st.MPI_TAG == TAG_FIN) {
            // El monitor nos indica que ya no habrá más items.
            printf("Consumidor %d: FIN recibido, termino.\n", id);
            break;
        }
        else if (st.MPI_TAG == TAG_RETRY) {
            // Buffer vacío por el momento. Reintentamos luego.
            usleep(5000);
            continue;
        }
        else if (st.MPI_TAG == TAG_CONSUME_ITEM) {
            // Recibimos un item válido para consumir.
            printf("Consumidor %d consumió %d\n", id, item);
            usleep(250000); // simulamos trabajo de consumo
        }
    }
}


// ======================================================================
// MAIN
// ======================================================================
int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int id, nproc;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);   // rank de este proceso
    MPI_Comm_size(MPI_COMM_WORLD, &nproc); // cantidad total de procesos

    // Necesitamos: 1 monitor + NPROD productores + NCONS consumidores
    if (nproc < 1 + NPROD + NCONS) {
        if (id == 0) {
            printf("ERROR: se necesitan al menos %d procesos.\n",
                   1 + NPROD + NCONS);
        }
        MPI_Finalize();
        return 0;
    }

    if (id == 0) {
        // Proceso 0 => monitor
        monitorMPI(nproc);
    }
    else if (id >= 1 && id <= NPROD) {
        // Procesos 1..NPROD => productores
        productorMPI(id);
    }
    else if (id > NPROD && id <= NPROD + NCONS) {
        // Procesos (NPROD+1)..(NPROD+NCONS) => consumidores
        consumidorMPI(id);
    }
    // Si hay más procesos de los necesarios, simplemente no hacen nada.

    MPI_Finalize();
    return 0;
}
