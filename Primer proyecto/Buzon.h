#pragma once

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <cstring>

#include "Potato.h"

#define KEY 0xB74200

class Buzon {
    private:
        int id;
        pid_t owner;

    public:
        Buzon();
        ~Buzon();

        ssize_t Enviar( const Potato &msg );
        ssize_t Recibir( Potato &msg, long type );
};
