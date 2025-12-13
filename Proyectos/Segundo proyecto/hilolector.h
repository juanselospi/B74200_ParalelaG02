#pragma once

#include <iostream>
#include <pthread.h>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <unistd.h>

#include "hilocontador.h"
#include "FileReader.h"

using namespace std;

// estructura para unidades de trabajo = bloques
struct Task {
    string data; // fragmento de 512 bytes
    int blockID;
    int assignedTo;
    int startLine;
    int endLine;
};

// cola compartida para repartir las tareas entre los contadores
struct TaskQueue {
    queue<Task> tasks; // tareas pndientes
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    bool done = false;
};

class HiloLector {
    private:
        string filePath;
        int numThreads;
        int strategy;

        FileReader* reader;

        vector< pthread_t > counterThreads;

        pthread_mutex_t mutex;

        // acumulador de etiquetas
        map< string, int > tagCounts;

        const size_t MAX_BUFFER = 512; // tama;o maximo de bloque por task
        
        TaskQueue taskQueue;

    public:
        HiloLector( string file, int counters, int strat );
        ~HiloLector();

        FileReader* getReader();

        string getFilePath();

        int getStrategy();
        int getNumThreads();

        map< string, int > counters();

        // para que los contadores actualizen resultados
        void updateTagCounts( const map< string, int > &localCounts);

        void printTagCount();
};
