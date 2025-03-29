#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct barrier {
    pthread_spinlock_t master;

    pthread_spinlock_t counter_lock;
    int liberando;
    volatile int waiting_processes;
    int n;
};

void barrier_increment_processes(struct barrier* b) {
    pthread_spin_lock(&b->counter_lock);
    b->waiting_processes++;
    pthread_spin_unlock(&b->counter_lock);
}

void barrier_decrement_processes(struct barrier* b) {
    pthread_spin_lock(&b->counter_lock);
    b->waiting_processes--;
    pthread_spin_unlock(&b->counter_lock);
}

int barrier_can_release(struct barrier* b) {
    int v;
    pthread_spin_lock(&b->counter_lock);
    v= b->waiting_processes == b->n;
    pthread_spin_unlock(&b->counter_lock);
    return v;
}

int barrier_can_stop_release(struct barrier* b) {
    int v;
    pthread_spin_lock(&b->counter_lock);
    v= b->waiting_processes == 0;
    pthread_spin_unlock(&b->counter_lock);
    return v;
}

int barrier_releasing(struct barrier* b) {
    int v;
    pthread_spin_lock(&b->counter_lock);
    v= b->liberando;
    pthread_spin_unlock(&b->counter_lock);
    return v;
}

void barrier_activate_releasing(struct barrier* b) {
    pthread_spin_lock(&b->counter_lock);
    b->liberando = 1;
    pthread_spin_unlock(&b->counter_lock);
}

void barrier_deactivate_releasing(struct barrier* b) {
    pthread_spin_lock(&b->counter_lock);
    b->liberando = 0;
    pthread_spin_unlock(&b->counter_lock);
}

void barrier_init(struct barrier* b, int n) {
    pthread_spin_init(&b->master,PTHREAD_PROCESS_PRIVATE);
    pthread_spin_init(&b->counter_lock,PTHREAD_PROCESS_PRIVATE);
    b->waiting_processes = 0;
    b->liberando = 0;
    b->n = n;
}

void barrier_wait(struct barrier* b) {
    while (barrier_releasing(b))
        ;
    int isAMaster = pthread_spin_trylock(&b->master) == 0;

    barrier_increment_processes(b);

    
    if(isAMaster)
    {
        while(!barrier_can_release(b));
        barrier_activate_releasing(b);
        pthread_spin_unlock(&b->master);
    }
    else
    {
        pthread_spin_lock(&b->master);
        pthread_spin_unlock(&b->master);
    }

    barrier_decrement_processes(b);
    
    if(barrier_can_stop_release(b))
        barrier_deactivate_releasing(b);
}