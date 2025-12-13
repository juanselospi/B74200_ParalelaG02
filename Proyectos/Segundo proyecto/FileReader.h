#pragma once

#include <iostream>
#include <pthread.h>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

class FileReader {
    private:
        ifstream filePath;
        pthread_mutex_t mutex;

        bool eof; // end of file: fin del archivo
        int numThreads;
        int strategy;
        int totalLines;
        int currentLine;

        vector<streampos> linePositions;

    public:
        FileReader( string file, int counters, int strat );
        ~FileReader();

        bool getNextLine( string &line );

        void preprocessFile();

        int getTotalLines();
        
        bool getLineAt( int lineIndex, string &line );

        // para dianamicas
        int getNextDynamicLine();
};
