#include <stdlib.h>
#include <semaphore.h>

//---------queue para thread---------------
struct thread_node_queue{
    sem_t s;
    struct thread_node_queue *next;
};

struct thread_queue{
    struct thread_node_queue *head;
    struct thread_node_queue *last;
};

struct thread_queue *create_queue(){
    struct thread_queue* queue = (struct thread_queue*) malloc(sizeof(struct thread_queue));
    queue->head = NULL;
    queue->last = NULL;

    return queue;
}

struct thread_node_queue *create_node_queue(){
    struct thread_node_queue *node_queue = (struct thread_node_queue*) malloc(sizeof(struct thread_node_queue));
    node_queue->next = NULL;
    sem_init(&node_queue->s, 0, 1);

    return node_queue;
}

int empty_queue(struct thread_queue* queue){
    return NULL == queue->head;
}

void insert_queues(struct thread_queue* queue, struct thread_node_queue* node){
    if(queue->head == NULL){
        queue->head = node;
        queue->last = node;
    }
    else if(queue->head == queue->last){
        queue->last = node;
        queue->head->next = node; 
    }
    else{

        queue->last->next = node;
        queue->last = node;
    }
}

struct thread_node_queue* pop_queue(struct thread_queue* queue){
    if(queue->head == NULL)
        return NULL;

    struct thread_node_queue* temp = queue->head;
    queue->head = temp->next;

    return temp;
}
//------------------------------------------
struct cond{
    sem_t x;
    sem_t m;
    struct thread_queue *waiters;
};

void wait(struct cond *CV){
    struct thread_node_queue* self = create_node_queue();

    sem_wait(&CV->x);
        insert_queues(CV->waiters, self);
    sem_post(&CV->x);

    sem_post(&CV->m);
    sem_wait(&self->s);
    sem_wait(&CV->m);
}

void signal(struct cond* CV){
    sem_wait(&CV->x);
    if(!empty_queue(CV->waiters)){
        struct thread_node_queue* temp = pop_queue(CV->waiters);
        sem_post(&temp->s);
        free(temp);
    }
    sem_post(&CV->x);
}

void brodcast(struct cond* CV){
    sem_wait(&CV->x);

    while(!empty_queue(CV->waiters)){
        struct thread_node_queue* temp = pop_queue(CV->waiters);
        sem_post(&temp->s);
        free(temp);
    }

    sem_post(&CV->x);
}
