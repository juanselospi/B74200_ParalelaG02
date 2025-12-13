#pragma once

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <omp.h>
#include <iostream>

#include "adn.h"

using namespace std;

class LCS {
    private:
        vector< vector< int >> matriz; // matriz para diagonales

    public:
        LCS();
        ~LCS();

        string serialLCS( string& S1 );



        string paralelaLCS( string& S1, int hilos);
};
