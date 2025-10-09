#include "Buzon.h"

Buzon::Buzon() {
    owner = getpid();
    this->id = msgget( KEY, IPC_CREAT | 0666 );
}

Buzon::~Buzon() {}

ssize_t Buzon::Enviar( const char *label, long counter, long type ) {
    struct Mensaje A;

    A.mtype = type;
    A.times = counter;

    strncpy( A.label, label, LABEL_SIZE - 1 );

    A.label[ LABEL_SIZE - 1 ] = '\0';

    return msgsnd( this->id, &A, sizeof( A ) - sizeof( long ), 0);
}

ssize_t Buzon::Enviar( const void *buffer, size_t len, long type ) {
    struct Mensaje *msg = ( struct Mensaje * )buffer;
    msg->mtype = type;

    return msgsnd( this->id, msg, len, 0 );
}

ssize_t Buzon::Recibir( void *buffer, size_t len, long type ) {
    return msgrcv( this->id, buffer, len, type, IPC_NOWAIT ); // IPC_NOWAIT evita que Recibir() se quede esperando mensajes en bucle, y solo considera los que ya estan en el buzon
}

void Buzon::Eliminar() {
    msgctl(this->id, IPC_RMID, NULL);
    printf("[Buzon] Cola de mensajes eliminada manualmente ( id = %d )\n", this->id);
}
