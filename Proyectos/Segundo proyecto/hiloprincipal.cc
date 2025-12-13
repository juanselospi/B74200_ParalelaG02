#include <iostream>
#include <pthread.h>
#include <vector>
#include <string>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>

#include "hilolector.h"

using namespace std;

// estructura para pasar parametros
struct LectorArgs
{
    string filePath;
    int numThreads;
    int strategy;
};

void startTimer( struct timeval * timerStart ) {
    gettimeofday( timerStart, NULL );
}

double getTimer( struct timeval timerStart ) {
    struct timeval timerStop, timerElapsed;
    gettimeofday( &timerStop, NULL );
    timersub( &timerStop, &timerStart, &timerElapsed );
    return timerElapsed.tv_sec * 1000.0 + timerElapsed.tv_usec / 1000.0;
}

void* executeLectors( void* arg ) {

    // convierto el puntero void generico para sacar datos de mi struct
    LectorArgs* args = ( LectorArgs* ) arg;

    // objeto lector con sus respectivos parametros
    HiloLector lector( args->filePath, args->numThreads, args->strategy );

    // acumulo en un map los conteos de mis lectores
    map< string, int >* tagResult = new map< string, int > ( lector.counters() );

    pthread_exit( ( void* ) tagResult );
}

int main( int argc, char* argv[] ) {

    // verifica que el usuario haga el input correcto
    if ( argc < 2 ) {
        cout << "Uso: ./bin/contar -t=<num_hilos> archivo1.html archivo2.html etc... -e=<estrategias>\n";
        return 1;
    }

    vector< string > files;
    int workerCount = 1; // contadores pedidos por el usuario 1 por defecto
    vector < int > strategies;

    // mapa para almacenar los resultados del conteo global
    map< string, int > globalTagCount;

    // contador de archivos validos
    size_t validFiles = 0;

    // para argumentos de entrada de la forma: contar -t=10 a.html b.html c.html -e=2,3,4
    for( int i = 1; i < argc; i++ ) {

        string arg = argv[ i ];

        // comprueba que el argumento actual del input inicie con -t=
        if( arg.rfind( "-t=", 0 ) == 0 ) {

            workerCount = stoi( arg.substr( 3 ) ); // extrae el 4to caracter o sea el numero y lo asigna a workerCount

            // manejo de valores nevativos para los trabajadores, si hay mas de 1 archivo no es tecnicamente serial pero meh... close enough.
            if( workerCount <= 0 ) {

                fprintf( stderr, "\nERROR: Invalid worker count '%d'. Parameter must be positive. Defaulting to serial ver.\n", workerCount );

                workerCount = 1;
            }

        // lo mismo pero con -e=
        } else if( arg.rfind( "-e=", 0 ) == 0 ) {

            // string para manejar la linea de estrategias
            string strat = arg.substr( 3 );
            size_t pos = 0;

            // busca las comas y extrae los numeros a la izquierda
            while( ( pos = strat.find( "," ) ) != string::npos ) {

                strategies.push_back( stoi( strat.substr( 0, pos ) ) );

                // y borro la coma
                strat.erase( 0, pos + 1 );
            }

            // cuando ya no hay comas guardo le ultimo numero que no entraria en el while
            strategies.push_back( stoi( strat ) );
        } else {

            // guardo los paths de los archivos
            files.push_back( arg );
        }
    }

    // imprimo lo que tengo hasta ahora
    cout << "\nSorting...\n" << "Files: " << files.size() << "\nUsing: " << workerCount << " counters per file" << endl;

    // para procesarlos asigno un hilo por archivo
    vector< pthread_t > lectorThreads( files.size() );
    vector< LectorArgs > lectorArgs( files.size() );

    // para medir le tiempo
    clock_t start, finish;
    struct timeval timerStart;
    double used, wused;

    // fase de medicion serial
    startTimer( &timerStart );
    start = clock();

    for( size_t i = 0; i < files.size(); i++ ) {

        ifstream testFile( files[ i ] );

        if( !testFile.good() ) {

            fprintf( stderr, "\nERROR: '%s' no existe o no se puede abrir. Se omitirá.\n", files[ i ].c_str() );
            continue;
        }

        testFile.close();

        // manejor de errores con la estrategia
        int strategy = 3;

        // si una estrategia no fue asignada se queda con la 1
        if( i < strategies.size()) {

            strategy = strategies[ i ];

            // validacion final de estrategia
            if( strategy < 1 || strategy > 4 ) {

                fprintf( stderr, "\nERROR: Invalid strategy '%d' for file '%s' defaulting to strategy 1\n", strategy, files[ i ].c_str() );

                strategy = 1;
            }

        } else {
            fprintf( stderr, "\nWARNING: Cannot find strategy for '%s'. Assigning default strategy 1...\n", files[ i ].c_str());

        }

        // se ejecuta cada archivo en serie con 1 solo hilo ( modo serial ) y su respectiva estrategia
        HiloLector lector( files[ i ], 1, strategy );
        map< string, int > localTags = lector.counters();

        // acumula resultados seriales
        for( const auto& tag : localTags ) {

            globalTagCount[ tag.first ] += tag.second;
        }
    }

    finish = clock();
    used = ( ( double ) ( finish - start ) ) / CLOCKS_PER_SEC;
    wused = getTimer( timerStart );
    double serialWallTime = wused;
    double serialCPUTime = used;

    // limpio resultados seriales
    globalTagCount.clear();

    // fase  medicion paralela
    startTimer( &timerStart );
    start = clock();

    // arranco los hilos
    for( size_t i = 0; i < files.size(); i++ ) {

        // comprobar si el archivo existe y que no este corrupto
        ifstream testFile( files[ i ] );

        if( !testFile.good() ) {

            fprintf( stderr, "\nERROR: '%s' doesnt exist or cannot be opened and will be omited\n", files[ i ].c_str() );
            continue;
        }

        testFile.close();

        // manejor de errores con la estrategia
        int strategy = 3;

        // si una estrategia no fue asignada se queda con la 1
        if( i < strategies.size()) {

            strategy = strategies[ i ];

            // validacion final de estrategia
            if( strategy < 1 || strategy > 4 ) {

                fprintf( stderr, "\nERROR: Invalid strategy '%d' for file '%s' defaulting to strategy 1\n", strategy, files[ i ].c_str() );

                strategy = 1;
            }

        } else {
            fprintf( stderr, "\nWARNING: Cannot find strategy for '%s'. Assigning default strategy 1...\n", files[ i ].c_str());

        }

        lectorArgs[ validFiles ] = { files[ i ], workerCount, strategy };
        pthread_create( &lectorThreads[ validFiles ], NULL, executeLectors, &lectorArgs[ validFiles ] );
        validFiles++;
    }

    // acumulo los tags cuando los hilos lectores termian su ejecucion
    for( int i = 0; i < validFiles; i++ ) {

        void* tagReturn;

        pthread_join( lectorThreads[ i ], &tagReturn );

        map< string, int >* tagPackage = static_cast< map< string, int >* >( tagReturn );

        // acumulo los tags que me retonaron en el acumulador global
        for( const auto& tag : *tagPackage ) {
            
            globalTagCount[ tag.first ] += tag.second;
        }

        // libero la mameria dinamica
        delete tagPackage;
    }

    finish = clock();
    used = ( ( double ) ( finish - start ) ) / CLOCKS_PER_SEC; // tiempo del CPU del hilo principal
    wused = getTimer( timerStart ); // tiempo de pared
    double parallelWallTime = wused;
    double parallelCPUTime = used;

    // imprimo los resultados una vez termino
    printf( "\nGlobal tag count:\n" );

    for( const auto& tag : globalTagCount ) {
        // printf( "IM IN");
        cout << tag.first << " : " << tag.second << endl;
    }

    printf( "\nWall time (serial): %.3f ms\n", serialWallTime );
    printf( "Main CPU time (serial): %.3f s\n", serialCPUTime );

    printf( "\nWall time (parallel): %.3f ms\n", parallelWallTime );
    printf( "Main CPU time (parallel): %.3f s\n", parallelCPUTime );

    double speedup = serialWallTime / parallelWallTime;
    printf( "\nSpeedUp: x%.3f\n", speedup );

    return 0;
}
