#include "Buzon.h"

Buzon::Buzon() {
    owner = getpid();
    this->id = msgget(KEY, IPC_CREAT | 0666);

}

ssize_t Buzon::Enviar(const char *label, long contador, long tipo) {
    struct Mensaje A;
    A.mtype = tipo;
    A.times = contador;
    strncpy(A.label, label, LABEL_SIZE -1);
    A.label[LABEL_SIZE - 1] = '\0';

    return msgsnd(this->id, &A, sizeof(A) - sizeof(long), 0);
}

ssize_t Buzon::Enviar(const void *buffer, size_t len, long tipo) {
    struct Mensaje *msg = (struct Mensaje *)buffer;
    msg->mtype = tipo;

    return msgsnd(this->id, msg, len, 0);
}

ssize_t Buzon::Recibir(void *buffer, size_t len, long tipo) {
    return msgrcv(this->id, buffer, len, tipo, 0);
}

Buzon::~Buzon() {
    // msgctl(this->id, IPC_RMID, NULL);
}
