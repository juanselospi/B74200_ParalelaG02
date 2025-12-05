#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;



/*
    MI COMPUTADORA TIENE:

    CPU: AMD Ryzen 9 8945HS (16) @ 5.26 GHz
    GPU 1: NVIDIA GeForce RTX 4060 Max-Q / Mobile [Discrete]
    GPU 2: AMD Radeon 780M Graphics [Integrated]
    Memory: 4.83 GiB / 30.59 GiB (16%)
    Swap: 0 B / 8.00 GiB (0%)
*/






#define FIBONACCI 5;

typedef struct {
    int valor1;
    int valor2;
    int fibo;
} Fibonacci;

const int TAG_fibo = 10; // mensajes normales
const int TAG_STOP = 9999;

int main(int argc, char** argv) {

    int miFibo = FIBONACCI;

    // parametros ingresados por el usuario
    if( argc > 1 ) {

        miFibo = atoi( argv[ 1 ] );

		// si me dan un valor invalido irme al default
		if(miFibo < 1) {

			miFibo = FIBONACCI;
		}
    }

    MPI_Init(&argc, &argv);


    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size); 


    Fibonacci fibon;

    // Cantidad de campos en el struct
    int blocklen[3] = {1, 1, 1};

    // Tipo de cada campo
    MPI_Datatype types[3] = {MPI_INT, MPI_INT, MPI_INT};

    // Desplazamientos de cada campo dentro de la struct
    MPI_Aint offsets[3];
    MPI_Aint base;


    MPI_Get_address(&fibon, &base);
    MPI_Get_address(&fibon.valor1, &offsets[0]);
    MPI_Get_address(&fibon.valor2, &offsets[1]);
    MPI_Get_address(&fibon.fibo, &offsets[2]);

    offsets[0] -= base;
    offsets[1] -= base;
    offsets[2] -= base;


    MPI_Datatype MPI_FIBO;
    MPI_Type_create_struct(3, blocklen, offsets, types, &MPI_FIBO);
    MPI_Type_commit(&MPI_FIBO);


    // PONER UN IF PARA CONTRARELOJ

    // DEFINIR PROCESO SIGUIENTE EN EL ANILLO
    int next = (rank + 1) % size; // siguiente proceso en el anillo
    int prev = (rank - 1 + size) % size; // proceso anterior

    MPI_Status st;

    // PROCESO 0 INICIA FIBONACCI
    if (rank == 0) {

        Fibonacci calculo;

        calculo.valor1 = 0;
        calculo.valor2 = 1;
        calculo.fibo = miFibo;

        //cout << calculo.fibo << " EL FIBO ORIGINAL" <<endl;

        printf("Proceso 0 inicia fibonacci con valores: \n%d\n%d\n%d\n", calculo.valor1, calculo.valor2, calculo.fibo);

        calculo.valor1 = calculo.valor2;
        calculo.valor2 = calculo.fibo;
        calculo.fibo = calculo.fibo - 1;

        printf("Proceso 0 realiza el primer fibonnaci con valores: \n%d\n%d\n%d\n", calculo.valor1, calculo.valor2, calculo.fibo);

        // Enviar al siguiente en el anillo
        MPI_Send(&calculo, 1, MPI_FIBO, next, TAG_fibo, MPI_COMM_WORLD);
    }



    // TODOS LOS PROCESOS: RECIBIR MENSAJES EN UN LOOP
    while (true) {

        Fibonacci calculo;

        // Esperar mensaje de la papa o mensaje STOP
        MPI_Recv(&calculo, 1, MPI_FIBO, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &st);

        int tag = st.MPI_TAG;

        //  CASO 1: RECIBO MENSAJE STOP → termino el proceso
        if (tag == TAG_STOP) {
            //printf("Proceso %d recibió STOP y termina con calores: \n%d\n%d\n%d\n", rank, calculo.valor1, calculo.valor2, calculo.fibo);

            // Reenviar mi señal de STOP al siguiente proceso (si no es el último en propagarlo)
            MPI_Send(&calculo, 1, MPI_FIBO, next, TAG_STOP, MPI_COMM_WORLD);
            break;
        }


        //  CASO 2: RECIBO LA PAPA NORMAL
        printf("Proceso %d recibió valores: \n%d\n%d\n%d\n", rank, calculo.valor1, calculo.valor2, calculo.fibo);

        // Actualizo los valores
        calculo.valor1 = calculo.valor2;

        calculo.valor2 = calculo.fibo;

        calculo.fibo = calculo.fibo - 1;

        //  SI VALOR LLEGA A 0 → mando la señal de parada!!!!!!!
        if (calculo.fibo <= 0) {

            printf("¡Fibonnaci se detuvo en el proceso %d\n", rank);

            // Enviar STOP por el anillo completo
            MPI_Send(&calculo, 1, MPI_FIBO, next, TAG_STOP, MPI_COMM_WORLD);

            break; // este proceso también termina
        }

        //  SI NO EXPLOTÓ, PASO AL SIGUIENTE
        MPI_Send(&calculo, 1, MPI_FIBO, next, TAG_fibo, MPI_COMM_WORLD);
    }

    MPI_Type_free(&MPI_FIBO);
    MPI_Finalize();
    return 0;

}