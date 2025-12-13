#include "hilolector.h"

struct CounterArgs
{
    HiloLector* lector;
    int counterID;
    TaskQueue* queue; // cola compartida
};

HiloLector::HiloLector( string file, int counters, int strat) : filePath( file ), numThreads( counters ), strategy( strat ) {

    reader = new FileReader( filePath, numThreads, strategy );
    pthread_mutex_init( &mutex, NULL );

    // para sincronizar la cola
    pthread_mutex_init( &taskQueue.mutex, NULL );
    pthread_cond_init( &taskQueue.not_empty, NULL );

    taskQueue.done = false;
}

HiloLector::~HiloLector() {

    delete reader;

    pthread_mutex_destroy( &mutex );
    pthread_mutex_destroy( &taskQueue.mutex );
    pthread_cond_destroy( &taskQueue.not_empty );
}

FileReader* HiloLector::getReader() {
    return reader;
}

string HiloLector::getFilePath() {
    return filePath;
}

int HiloLector::getStrategy() {
    return strategy;
}

int HiloLector::getNumThreads() {
    return numThreads;
}

// para asignar los bloques a mis contadores
int assignBlockToThread( int blockID, int numThreads, int strategy, int totalBlocks ) {

    switch( strategy ) {
        // el casi 1 lo voy  manejar como esttatico por bloques
        case 1: {

            int blocksPerThread = totalBlocks / numThreads;

            if( blockID >= blocksPerThread * numThreads ) {

                return numThreads - 1;
            }
    
            return blockID / blocksPerThread;
        }
        // mi caso 2 es estatico ciclico
        case 2:
            return blockID % numThreads;
        // el caso 3 sera dianamico y la estrategia que usar por defecto al ser la mas facil de implementar
        case 3:
        default:
            return -1;
    }
}

// usar modelo productor - consumidor para manejar la oferta y demanda de tasks
// mi consumidor va a procesar bloques definidos por rango de líneas
void* executeCounters( void* arg ) {
    CounterArgs* args = ( CounterArgs* ) arg;   // casteo de toda la vida
    HiloLector* lector = args->lector;
    TaskQueue* queue = args->queue;
    int id = args->counterID;

    map< string, int > localCounts;
    string carryOver; // para manejar casos con etiquetas partidas las almaceno en este string

    while( true ) {

        pthread_mutex_lock( &queue->mutex );

        while( queue->tasks.empty() && !queue->done ) {

            pthread_cond_wait( &queue->not_empty, &queue->mutex );
        }
        if( queue->tasks.empty() && queue->done ) {

            pthread_mutex_unlock( &queue->mutex );
            break;
        }

        Task task = queue->tasks.front();

        if( task.assignedTo != -1 && task.assignedTo != id ) {

            pthread_mutex_unlock( &queue->mutex );
            sched_yield();
            continue;
        }

        queue->tasks.pop();

        // pthread_cond_signal( &queue->not_full );
        pthread_mutex_unlock( &queue->mutex );

        // proceso el carryOver interno para etiquetas cortadas
        countTagsBuffer( task.data, localCounts, carryOver );
    }

    // por si queda algo dentro del string carryOver
    if( !carryOver.empty() ) {

        countTagsBuffer( "", localCounts, carryOver );
    }

    // finalmente actualizo mi conteo
    lector->updateTagCounts( localCounts );
    pthread_exit(NULL);
}

// los bloques seran manejados segun el numero de contadores que el usuario me pida
map< string, int > HiloLector::counters() {

    counterThreads.resize( numThreads );
    vector< CounterArgs > args( numThreads ); // reminder: no usar static porque se encicla

    for( int i = 0; i < numThreads; i++ ) {

        args[ i ] = { this, i, &taskQueue };
        pthread_create( &counterThreads[ i ], nullptr, executeCounters, &args[ i ] );
    }

    int totalLines = reader->getTotalLines();
    if( totalLines == 0 ) {

        pthread_mutex_lock( &taskQueue.mutex );
        taskQueue.done = true;

        pthread_cond_broadcast( &taskQueue.not_empty );
        pthread_mutex_unlock( &taskQueue.mutex );

        for( auto& counter : counterThreads ) {
            
            pthread_join( counter, nullptr );
        }

        return tagCounts;
    }

    // agregar estrategia 4 cuando termine las otras 3...  hacer bloques de 512 bytes y mandarlos a procesar cuidando las etiquetas cortadas algo parecido a dinamico pero por bloques
    if( strategy == 4 ) {

        size_t filePos = 0;
        string line;

        for( int i = 0; i < totalLines; i++ ) {

            if( !reader->getLineAt( i, line ) ) {
                continue;
            }

            size_t offset = 0;

            while( offset < line.size() ) {
                
                Task task;
                task.startLine = i;
                task.endLine = i + 1;
                task.assignedTo = -1;

                size_t chunkSize = min( MAX_BUFFER, line.size() - offset );
                task.data = line.substr( offset, chunkSize );
                offset += chunkSize;

                pthread_mutex_lock( &taskQueue.mutex );
                taskQueue.tasks.push( task );
                pthread_cond_signal( &taskQueue.not_empty );
                pthread_mutex_unlock( &taskQueue.mutex );
            }
        }
    } else { 
        // para las estrategias 1 2 y 3
        for( int i = 0; i < totalLines; i++ ) {

            string line;

            if ( !reader->getLineAt( i, line ) ) {
                continue;
            }

            size_t offset = 0;

            while( offset < line.size() ) {

                Task task;
                task.startLine = i;
                task.endLine = i + 1;

                if (strategy == 1) {
                    // estrategia 1 por bloques
                    task.assignedTo = (i * numThreads) / totalLines;
                } else if (strategy == 2) {
                    // estrategia 2 ciclica
                    task.assignedTo = i % numThreads;
                } else {
                    // estrategia 3 dinamica
                    task.assignedTo = -1;
                }

                size_t chunkSize = min( MAX_BUFFER, line.size() - offset );
                task.data = line.substr( offset, chunkSize ); 
                offset += chunkSize;

                pthread_mutex_lock( &taskQueue.mutex );
                taskQueue.tasks.push( task );
                pthread_cond_signal( &taskQueue.not_empty );
                pthread_mutex_unlock( &taskQueue.mutex );
            }
        }
    }

    pthread_mutex_lock(&taskQueue.mutex);
    taskQueue.done = true;
    pthread_cond_broadcast(&taskQueue.not_empty);
    pthread_mutex_unlock(&taskQueue.mutex);

    // espero a los contadores
    for( auto& counter : counterThreads ) {

        pthread_join( counter, nullptr );
    }

    return tagCounts;
}

void HiloLector::updateTagCounts( const map< string, int > &localCounts ) {

    // bloquea la lectura
    pthread_mutex_lock( &mutex );

    for( const auto& tag : localCounts ) {
        
        tagCounts[ tag.first ] += tag.second;
    }

    // desbloquea el hilo
    pthread_mutex_unlock( &mutex );

}

void HiloLector::printTagCount() {

    std::cout << "\nCount for: " << filePath << ":\n";

    for( const auto& tag : tagCounts ) {

        std::cout << tag.first << " : " << tag.second << endl;
    }

    cout << endl;
}
