/***************************************************************
 *  🧨 PROBLEMA: Papa Bomba en Anillo con MPI_Datatype
 *  ------------------------------------------------------------
 *  ENUNCIADO:
 *  N procesos forman un anillo (0 → 1 → 2 → ... → N−1 → 0).
 *  
 *  El proceso 0 inicia la papa con:
 *      - valor inicial (ej: 10)
 *      - hops = 0
 *
 *  Cada vez que la papa llega a un proceso:
 *      1) Se imprime quién la recibió, el valor, y los hops.
 *      2) Se decrementa el valor en 1.
 *      3) Se incrementa hops en 1.
 *
 *  Si valor llega a 0 → ¡LA BOMBA EXPLOTA!
 *      - Se imprime un mensaje especial.
 *      - Se envía un mensaje de STOP por el anillo completo.
 *      - Cada proceso, al recibir STOP, imprime que terminó.
 *
 *  Este ejercicio demuestra:
 *      ✔ Comunicación en anillo
 *      ✔ MPI_Send + MPI_Recv
 *      ✔ Uso de MPI_Datatype para enviar structs
 *      ✔ Control seguro de finalización con STOP en anillo
 ***************************************************************/

#include <mpi.h>
#include <cstdio>
#include <cstdlib>

//
// =============================================================
//  ESTRUCTURA DE LA PAPA (mensaje enviado entre procesos)
// =============================================================
//  Enviaremos este struct como un solo objeto MPI.
//
typedef struct {
    int valor;   // valor restante de la papa
    int hops;    // saltos realizados
} Papa;

// TAGS
const int TAG_PAPA = 10;   // mensajes normales de la papa
const int TAG_STOP = 9999; // TAG seguro para detener el anillo (MPI exige TAG ≥ 0)


// =============================================================
//  PROGRAMA PRINCIPAL
// =============================================================
int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Identificador del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Número total de procesos en el anillo

    // ---------------------------------------------------------
    // DEFINICIÓN DEL MPI_Datatype PARA LA STRUCT "Papa"
    // ---------------------------------------------------------

    Papa ejemplo;

    // Cantidad de campos en el struct
    int blocklen[2] = {1, 1};

    // Tipo de cada campo
    MPI_Datatype types[2] = {MPI_INT, MPI_INT};

    // Desplazamientos de cada campo dentro de la struct
    MPI_Aint offsets[2];
    MPI_Aint base;

    MPI_Get_address(&ejemplo, &base);
    MPI_Get_address(&ejemplo.valor, &offsets[0]);
    MPI_Get_address(&ejemplo.hops, &offsets[1]);

    offsets[0] -= base;
    offsets[1] -= base;

    MPI_Datatype MPI_PAPA;
    MPI_Type_create_struct(2, blocklen, offsets, types, &MPI_PAPA);
    MPI_Type_commit(&MPI_PAPA);


    // ---------------------------------------------------------
    // DEFINIR PROCESO SIGUIENTE EN EL ANILLO
    // ---------------------------------------------------------
    int next = (rank + 1) % size;     // siguiente proceso en el anillo
    int prev = (rank - 1 + size) % size; // proceso anterior (no se usa, pero sirve para entender el anillo)

    MPI_Status st;

    // ---------------------------------------------------------
    // PROCESO 0 INICIA LA PAPA
    // ---------------------------------------------------------
    if (rank == 0) {

        Papa bomba;
        bomba.valor = 10;  // valor inicial (puedes cambiarlo)
        bomba.hops = 0;

        printf("Proceso 0 inicia la papa con valor = %d\n", bomba.valor);

        // Enviar al siguiente en el anillo
        MPI_Send(&bomba, 1, MPI_PAPA, next, TAG_PAPA, MPI_COMM_WORLD);
    }


    // ---------------------------------------------------------
    // TODOS LOS PROCESOS: RECIBIR MENSAJES EN UN LOOP
    // ---------------------------------------------------------
    while (true) {

        Papa bomba;

        // Esperar mensaje de la papa o mensaje STOP
        MPI_Recv(&bomba, 1, MPI_PAPA, MPI_ANY_SOURCE,
                 MPI_ANY_TAG, MPI_COMM_WORLD, &st);

        int tag = st.MPI_TAG;

        // =====================================================
        //  CASO 1: RECIBO MENSAJE STOP → termino el proceso
        // =====================================================
        if (tag == TAG_STOP) {
            printf("Proceso %d recibió STOP y termina.\n", rank);

            // Reenviar STOP al siguiente proceso (si no soy el último en propagarlo)
            MPI_Send(&bomba, 1, MPI_PAPA, next, TAG_STOP, MPI_COMM_WORLD);
            break;
        }


        // =====================================================
        //  CASO 2: RECIBO LA PAPA NORMAL
        // =====================================================
        printf("Proceso %d recibió valor=%d desde %d (hops=%d)\n",
               rank, bomba.valor, st.MPI_SOURCE, bomba.hops);

        // Decrementar el valor
        bomba.valor--;
        bomba.hops++;

        // -----------------------------------------------------
        //  SI VALOR LLEGA A 0 → ¡EXPLOTA!
        // -----------------------------------------------------
        if (bomba.valor <= 0) {

            printf("💥💥 ¡¡BOOM!! La bomba explotó en el proceso %d 💥💥\n", rank);

            // Enviar STOP por el anillo completo
            MPI_Send(&bomba, 1, MPI_PAPA, next, TAG_STOP, MPI_COMM_WORLD);

            break;  // este proceso también termina
        }

        // -----------------------------------------------------
        //  SI NO EXPLOTÓ, ENVÍO LA PAPA AL SIGUIENTE
        // -----------------------------------------------------
        MPI_Send(&bomba, 1, MPI_PAPA, next, TAG_PAPA, MPI_COMM_WORLD);
    }

    MPI_Type_free(&MPI_PAPA);
    MPI_Finalize();
    return 0;
}
