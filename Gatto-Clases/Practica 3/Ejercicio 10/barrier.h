#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct barrier {
    pthread_spinlock_t mutex;
    int waiting_processes;
    int n;
};

void barrier_init(struct barrier* b, int n) {
    pthread_spin_init(&b->mutex, 1);
    b->waiting_processes = 0;
    b->n = n;
}

void barrier_wait(struct barrier *b) {
    pthread_spin_lock(&b->mutex);
    if(b->waiting_processes == b->n-1)
    {
        // Soy el n-esimo hilo que llega a la barrera
        b->waiting_processes = 0;
    }
    else
    {
        // Llegue a la barrera antes, espero al siguiente
        b->waiting_processes++;
    }
    // Libero el lock
    pthread_spin_unlock(&b->mutex);
}