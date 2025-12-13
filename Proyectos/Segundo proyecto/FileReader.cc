#include "FileReader.h"

FileReader::FileReader( string file, int counters, int strat ) : eof( false ), numThreads( counters ), strategy( strat ), totalLines( 0 ), currentLine( 0 ) {
    
    filePath.open( file );

    if( !filePath.is_open() ) {

        cerr << "\nERROR! cant open this file: " << file << endl;
        exit( 1 );
    }

    pthread_mutex_init( &mutex, NULL );

    preprocessFile();
}

FileReader::~FileReader() {

    if( filePath.is_open() ) {

        filePath.close();
    }

    pthread_mutex_destroy( &mutex );
}

// cada vez que un hilo llama esta funcion aseguro exclusion con el mutex
bool FileReader::getNextLine( string &line ) {

    pthread_mutex_lock( &mutex );

    if( eof == true || !getline( filePath, line ) ) {
        eof = true;

        pthread_mutex_unlock( &mutex );
        return false;
    }

    pthread_mutex_unlock( &mutex );
    return true;
}

void FileReader::preprocessFile() {

    string dummy;
    streampos pos;

    filePath.clear();
    filePath.seekg( 0, ios::beg );

    while ( true ) {

        pos = filePath.tellg();

        if( !getline(filePath, dummy) ) {
            break;
        }

        linePositions.push_back( pos );
        totalLines++;
    }

    filePath.clear();
    filePath.seekg( 0, ios::beg );
}

int FileReader::getTotalLines() {
    return totalLines;
}

bool FileReader::getLineAt( int lineIndex, string &line ) {

    if( lineIndex < 0 || lineIndex >= totalLines ) {

        return false;
    }

    pthread_mutex_lock( &mutex );

    filePath.clear();
    filePath.seekg( linePositions[ lineIndex ] );
    getline( filePath, line );

    pthread_mutex_unlock( &mutex );

    return true;
}


int FileReader::getNextDynamicLine() {
    
    pthread_mutex_lock( &mutex );

    if( currentLine >= totalLines ) {

        pthread_mutex_unlock( &mutex );
        return -1;
    }

    int lineIndex = currentLine;
    currentLine++;

    pthread_mutex_unlock( &mutex );

    return lineIndex;
}
