#include <semaphore.h>

struct channel {
    sem_t write,read;
    pthread_mutex_t mutex;
    int v;
};

void channel_init(struct channel *c) {
    sem_init(&c->read, 0, 0);
    sem_init(&c->write, 0, 0);
    pthread_mutex_init(&c->mutex, NULL);
}

void chan_write(struct channel *c, int v) {
    // Espero a que halla alguien que lea
    sem_wait(&c->read);
    
    pthread_mutex_lock(&c->mutex);
    // Escribo
    c->v = v;
    pthread_mutex_unlock(&c->mutex);

    // Ya escribi
    sem_post(&c->write);
}

int chan_read(struct channel *c) {
    // Quiero leer un valor
    sem_post(&c->read);
    
    // Espero al escritor
    sem_wait(&c->write);

    // Leo el valor
    pthread_mutex_lock(&c->mutex);
    int val = c->v;
    pthread_mutex_unlock(&c->mutex);
    return val;
}