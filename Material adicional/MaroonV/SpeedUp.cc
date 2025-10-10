#include <iostream>       // Para std::cout, imprimir resultados
#include <pthread.h>      // Librería para trabajar con hilos POSIX
#include <vector>         // No se usa realmente en tu código, se puede eliminar
#include <algorithm>      // Para std::fill
#include <unistd.h>       // Para sleep() que simula tiempo de tarea

#define HILOS 4           // Número de hilos/trabajadores
#define TAREAS 14         // Número total de tareas

// Tiempos de cada tarea
int tasks[ TAREAS ] = { 1, 2, 2, 1, 1, 1, 2, 1, 2, 2, 1, 2, 1, 2 };

// Para guardar el tiempo total de cada hilo
int times[ HILOS ] = { 0 };

// Para mapeo dinamico
int dynamicIndex = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


// Calcula el SpeedUp dado el array times de los hilos

double calcularSpeedUp( int times[], int numHilos ) {
    int maxTime = 0; // Variable para guardar el hilo más lento

    for( int i = 0; i < numHilos; i++ ) {
        if( times[ i ] > maxTime ) { // Busca el máximo de times
            maxTime = times[ i ];
        }
    }

    int totalSerial = 0; // Suma de todas las tareas en serie

    for( int i = 0; i < TAREAS; i++ ) {
        totalSerial += tasks[ i ];
    }

    return static_cast<double>( totalSerial ) / maxTime; // SpeedUp = serial / paralelo
}


// -------------------- Funciones de mapeo --------------------

// Estático por bloques
void* hilo_bloque( void* arg ) {
    int id = *( int* )arg;  // Convertir el puntero a int y obtener el id del hilo
    int inicio = ( TAREAS / HILOS ) * id; // Calcula inicio del bloque
    int fin = ( id == HILOS - 1 ) ? TAREAS : inicio + ( TAREAS / HILOS ); // Calcula fin del bloque
    
    for( int i = inicio; i < fin; i++ ) {
        times[ id ] += tasks[ i ];   // Suma el tiempo de la tarea al hilo
        sleep( tasks[ i ] );         // Simula tiempo de ejecución
    }

    pthread_exit( 0 );               // Termina el hilo
}


// Estático ciclico
void* hilo_ciclico( void* arg ) {
    int id = *( int* )arg; // Convertir el puntero a int y obtener el id del hilo

    // Clave del mapeo cíclico:

    // Empieza en la posición id del array de tareas.

    // Salta cada HILOS posiciones (i += HILOS).

    // Ejemplo con 4 hilos y 14 tareas:

    // Hilo 0: toma tareas 0, 4, 8, 12

    // Hilo 1: toma tareas 1, 5, 9, 13

    // Hilo 2: toma tareas 2, 6, 10

    // Hilo 3: toma tareas 3, 7, 11

    // De esta forma, las tareas se reparten de manera equilibrada y cíclica, evitando que un hilo tenga todas las tareas largas juntas.

    for( int i = id; i < TAREAS; i += HILOS ) {
        times[ id ] += tasks[ i ];  // Suma el tiempo de la tarea correspondiente al hilo
        sleep( tasks[ i ] );        // Simula la ejecución de la tarea

    }
    
    pthread_exit( 0 );
}

// Dinamico
void* hilo_dinamico( void* arg ) {
    int id = *( int* )arg; // Convertir el puntero a int y obtener el id del hilo
    
    while( true ) {
        pthread_mutex_lock( &mutex ); // Bloquea acceso al contador compartido
        
        if( dynamicIndex >= TAREAS ) { // Si ya no hay tareas
            pthread_mutex_unlock( &mutex ); // Liberamos el mutex
            break; // Salimos del bucle y terminamos el hilo
        }
        
        int t = dynamicIndex; // Guarda la tarea actual
        dynamicIndex++; // Avanza el contador a la siguiente tarea
        pthread_mutex_unlock( &mutex ); // Libera el mutex


        times[ id ] += tasks[ t ]; // Suma el tiempo de la tarea asignada dinámicamente
        sleep( tasks[ t ] ); // Simula la ejecución
    }

    pthread_exit( 0 );
}

// -------------------- Main --------------------

int main() {
    pthread_t hilos[ HILOS ]; // Array para guardar los identificadores de los hilos
    int ids[ HILOS ]; // Array para guardar los IDs de los hilos (0,1,2,3)

    std::cout << "Analizando... el tiempo de combate es de..." << std::endl;

    // -------- Ejemplo con mapeo estático por bloques --------

    std::fill( times, times + HILOS, 0 ); // Reinicia los tiempos de todos los hilos a 0

    for( int i = 0; i < HILOS; i++ ) {
        ids[ i ] = i; // Asignamos el ID del hilo (0 a HILOS-1)
        pthread_create( &hilos[ i ], NULL, hilo_bloque, &ids[ i ] ); // Creamos el hilo
    }

    for( int i = 0; i < HILOS; i++ ) {
        pthread_join( hilos[ i ], NULL ); // Espera que cada hilo termine antes de continuar
    }

    std::cout << "Tiempos por hilo (bloque): ";

    for( int i = 0; i < HILOS; i++ ) {
        std::cout << times[ i ] << " "; // Imprime el tiempo total que cada hilo tardó
    }

    std::cout << std::endl;

    // Imprimir SpeedUp
    double speedupBloque = calcularSpeedUp( times, HILOS );
    std::cout << "SpeedUp (bloque): " << speedupBloque << std::endl;


    // -------- Ejemplo con mapeo cíclico --------

    std::fill( times, times + HILOS, 0 ); // Reinicia los tiempos a 0

    for( int i = 0; i < HILOS; i++ ) {
        ids[ i ] = i; 
        pthread_create( &hilos[ i ], NULL, hilo_ciclico, &ids[ i ] ); // Cada hilo ejecuta el mapeo cíclico
    }

    for( int i = 0; i < HILOS; i++ ) {
        pthread_join( hilos[ i ], NULL ); // Espera que todos terminen
    }

    std::cout << "Tiempos por hilo (ciclico): ";

    for( int i = 0; i < HILOS; i++ ) {
        std::cout << times[ i ] << " "; // Imprime resultados
    }
    std::cout << std::endl;

    // Imprimir SpeedUp
    double speedupCiclico = calcularSpeedUp( times, HILOS );
    std::cout << "SpeedUp (ciclico): " << speedupCiclico << std::endl;

    // -------- Ejemplo con mapeo dinámico --------

    std::fill( times, times + HILOS, 0 ); // Reinicia tiempos
    dynamicIndex = 0; // Reinicia el índice de tareas para que todos comiencen desde el inicio

    for( int i = 0; i < HILOS; i++ ) {
        ids[ i ] = i;
        pthread_create( &hilos[ i ], NULL, hilo_dinamico, &ids[ i ] ); // Cada hilo ejecuta la función dinámica
    }

    for( int i = 0; i < HILOS; i++ ) {
        pthread_join( hilos[ i ], NULL ); // Espera a que todos terminen
    }

    std::cout << "Tiempos por hilo (dinamico): ";

    for( int i = 0; i < HILOS; i++ ) {
        std::cout << times[ i ] << " ";  // Imprime los tiempos finales de cada hilo
    }

    std::cout << std::endl;

    // Imprimir SpeedUp
    double speedupDinamico = calcularSpeedUp( times, HILOS );
    std::cout << "SpeedUp (dinamico): " << speedupDinamico << std::endl;

    return 0;
}

// Un SpeedUp con valor mayor de eficiencia quiere decir que es X veces mas rapido que el serial
