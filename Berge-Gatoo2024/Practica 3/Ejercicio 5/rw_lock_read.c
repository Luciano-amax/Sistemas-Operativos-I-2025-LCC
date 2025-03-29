#include "rw_lock.h"
#include <semaphore.h>

struct _rwlock_t {
    sem_t reading;
    sem_t writing;
    sem_t readers;
};

int rwlock_get_readers(rwlock_t* l) {
    int readers;
    sem_getvalue(&l->readers, &readers);
    return readers;
}

void rwlock_init(rwlock_t* l) {
    sem_init(&l->reading, 0, 1);
    sem_init(&l->writing, 0, 1);
    sem_init(&l->readers, 0, 0);
}

void rwlock_rdlock(rwlock_t* l) {
    // Espero a poder escribir
    sem_wait(&l->reading);

    // Aumento la cantidad de lectores
    sem_post(&l->readers);

    // Si soy el primero en entrar, tengo que esperar a que
    // se termine de escribir y luego evitar que entren
    // escritores
    if(rwlock_get_readers(l) == 1)
        sem_wait(&l->writing);

    // El siguiente escritor puede pasar
    sem_post(&l->reading);
}

void rwlock_wrlock(rwlock_t* l) {
    sem_wait(&l->writing);
}

void rwlock_unlock(rwlock_t* l) {
    // No dejar pasar a los escritores y lectores
    sem_trywait(&l->reading);
    sem_trywait(&l->writing);

    // Si un escritor llama a unlock, la cantidad de lectores ya es 0
    // asi que esta linea no cambia.
    // Si un lector llama a unlock, la cantidad de escritores baja
    sem_trywait(&l->readers);

    sem_post(&l->reading);
    sem_post(&l->writing);
}

/*
    Los eescritores pueden sufrir stravation porque puede pasar que los lectores nunca dejen
    de leer, haciendo que nunca se pueda escribir.
*/
