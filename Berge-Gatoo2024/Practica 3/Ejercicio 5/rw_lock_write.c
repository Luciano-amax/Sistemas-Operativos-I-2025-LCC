#include "rw_lock.h"
#include <pthread.h>

struct _rwlock_t {
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    unsigned waiting_writers;
    unsigned is_writing;
    unsigned active_readers;
};


void rwlock_init(rwlock_t* l) {
    pthread_mutex_init(&l->mutex, NULL);
    pthread_cond_init(&l->cond, NULL);
    l->waiting_writers = 0;
    l->is_writing = 0;
    l->active_readers = 0;
}

void rwlock_rdlock(rwlock_t* l) {
    pthread_mutex_lock(&l->mutex);
    while(l->waiting_writers > 0 || l->is_writing)
        pthread_cond_wait(&l->cond, &l->mutex);
    l->active_readers++;
    pthread_mutex_unlock(&l->mutex);
}

void rwlock_wrlock(rwlock_t* l) {
    pthread_mutex_lock(&l->mutex);
    l->waiting_writers++;
    while(l->active_readers>0 || l->is_writing)
        pthread_cond_wait(&l->cond, &l->mutex);
    l->waiting_writers--;
    l->is_writing = 1;
    pthread_mutex_unlock(&l->mutex);
}

void rwlock_unlock(rwlock_t* l) {
    pthread_mutex_lock(&l->mutex);
    if(l->active_readers > 0) {
        // A reader calls unlock
        l->active_readers--;
        if(l->active_readers == 0)
            pthread_cond_broadcast(&l->cond);
    }
    else
    {
        // A writer calls unlock
        l->is_writing = 0;  
        pthread_cond_broadcast(&l->cond);
    }
    pthread_mutex_unlock(&l->mutex);
}
