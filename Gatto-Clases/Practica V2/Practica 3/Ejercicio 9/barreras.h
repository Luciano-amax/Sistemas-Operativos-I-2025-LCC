#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>


struct barrier {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int numero_total_procesos;
    int numero_procesos_esperando;
};

void barrier_init(struct barrier* b, int n){
    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->cond, NULL);
    b->numero_procesos_esperando = 0;
    b->numero_total_procesos = n;
}

void barrier_wait(struct barrier *b){
    pthread_mutex_lock(&b->mutex);
    
    if(b->numero_procesos_esperando == b->numero_total_procesos - 1){
        pthread_cond_broadcast(&b->cond);
    }
    else{
        b->numero_procesos_esperando++;
        pthread_cond_wait(&b->cond, &b->mutex);
    }

    pthread_mutex_unlock(&b->mutex);
}
