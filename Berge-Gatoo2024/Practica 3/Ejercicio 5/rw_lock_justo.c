#include "rw_lock.h"
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

// Pongo los threads en una queue, los threads van ingresando en orden de llegada

enum _rw_thread_type {
    RW_THREAD_READ,
    RW_THREAD_WRITE,
};

struct _rw_thread {
    pthread_t id;
    enum _rw_thread_type type;
};

struct _rw_thread_queue_node {
    struct _rw_thread thread;
    struct _rw_thread_queue_node* sig;
};

struct _rw_thread_queue {
    struct _rw_thread_queue_node* head, *last;
    pthread_mutex_t mutex;
};

void _rw_thread_queue_init(struct _rw_thread_queue* q) {
    q->head = q->last = NULL;
    pthread_mutex_init(&q->mutex, NULL);
}

void _rw_thread_queue_push(struct _rw_thread_queue* q, enum _rw_thread_type type) {
    pthread_mutex_lock(&q->mutex);
    struct _rw_thread_queue_node* new_node = (struct _rw_thread_queue_node *)malloc(sizeof(struct _rw_thread_queue_node));

    if(new_node == NULL)
        exit(EXIT_FAILURE);

    new_node->thread.id = pthread_self();
    new_node->thread.type = type;
    new_node->sig = NULL;
    if(q->head==NULL && q->last == NULL) {
        q->head = new_node;
        q->last = new_node;

    }
    else if(q->head == q->last) {
        q->last = new_node;
        q->head->sig = q->last;
    assert(q->head != NULL);
    }
    else
    {
        q->last->sig = new_node;
        q->last = new_node;
    }
    pthread_mutex_unlock(&q->mutex);
}

struct _rw_thread _rw_thread_queue_front(struct _rw_thread_queue* q) {
    pthread_mutex_lock(&q->mutex);
    struct _rw_thread thread = q->head->thread;
    pthread_mutex_unlock(&q->mutex);
    return thread;
}

void _rw_thread_queue_pop(struct _rw_thread_queue* q) {
    pthread_mutex_lock(&q->mutex);
        struct _rw_thread_queue_node* old_node = q->head;
        if(q->head==q->last)
            q->head = q->last = NULL;
        else
            q->head = q->head->sig;
        free(old_node);
    pthread_mutex_unlock(&q->mutex);
}



struct _rwlock_t {
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    unsigned waiting_writers;
    unsigned is_writing;
    unsigned active_readers;

    struct _rw_thread_queue queue;
};


void rwlock_init(rwlock_t* l) {
    pthread_mutex_init(&l->mutex, NULL);
    pthread_cond_init(&l->cond, NULL);
    _rw_thread_queue_init(&l->queue);
    l->waiting_writers = 0;
    l->is_writing = 0;
    l->active_readers = 0;
}

void rwlock_rdlock(rwlock_t* l) {
    _rw_thread_queue_push(&l->queue, RW_THREAD_READ);

    pthread_mutex_lock(&l->mutex);

    pthread_t current_thread_id = pthread_self();
    
    while(current_thread_id != _rw_thread_queue_front(&l->queue).id || l->is_writing)
        pthread_cond_wait(&l->cond, &l->mutex);
    
    l->active_readers++;
    pthread_mutex_unlock(&l->mutex);

    
    _rw_thread_queue_pop(&l->queue);
}

void rwlock_wrlock(rwlock_t* l) {
    _rw_thread_queue_push(&l->queue, RW_THREAD_WRITE);

    pthread_mutex_lock(&l->mutex);    

    pthread_t current_thread_id = pthread_self();

    l->waiting_writers++;
    
    while(current_thread_id != _rw_thread_queue_front(&l->queue).id || l->active_readers>0 || l->is_writing)
        pthread_cond_wait(&l->cond, &l->mutex);

    l->waiting_writers--;
    l->is_writing = 1;
    pthread_mutex_unlock(&l->mutex);

    _rw_thread_queue_pop(&l->queue);
}

void rwlock_unlock(rwlock_t* l) {
    pthread_mutex_lock(&l->mutex);
    if(l->active_readers > 0) {
        // Lector llama a unlock
        l->active_readers--;
        if(l->active_readers == 0)
            pthread_cond_broadcast(&l->cond);
    }
    else
    {
        // Escritor llama a unlock
        l->is_writing = 0;  
        pthread_cond_broadcast(&l->cond);
    }
    pthread_mutex_unlock(&l->mutex);
}
