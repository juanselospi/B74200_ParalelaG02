#include "Buzon.h"

Buzon::Buzon() {
    owner = getpid();
    this->id = msgget( KEY, IPC_CREAT | 0666);
}

Buzon::~Buzon() {
    if( getpid() == owner ) {
        msgctl( this->id, IPC_RMID, NULL );
    }
}

ssize_t Buzon::Enviar( const Potato &msg ) {
    return msgsnd( this->id, &msg, sizeof( Potato ) - sizeof( long ), 0 );
}

ssize_t Buzon::Recibir( Potato &msg, long type ) {
    return msgrcv( this->id, &msg, sizeof( Potato ) - sizeof( long ), type, 0 );
}
