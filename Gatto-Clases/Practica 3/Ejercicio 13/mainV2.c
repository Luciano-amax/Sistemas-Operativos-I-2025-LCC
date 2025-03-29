#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 10

#define PRODUCTORES 5
#define CONSUMIDORES 5

int* buffer[BUFFER_SIZE];
int num = 0, add = 0, rem = 0;

//-----------------------

sem_t x, m;
sem_t lock;

//---------queue para thread---------------
struct threadNodeQueue{
    sem_t s;
    sem_t x;                        //Semaforo global (sirve de lock)
    struct threadNodeQueue *next;
};

struct threadQueue{
    struct threadNodeQueue *head;
    struct threadNodeQueue *last;
};

struct threadQueue *createQueue(){
    struct threadQueue* queue = malloc(sizeof(struct threadQueue));
    queue->head = NULL;
    queue->last = NULL;

    return queue;
}

struct threadNodeQueue *createNodeQueue(){
    struct threadNodeQueue *NodeQueue = malloc(sizeof(struct threadNodeQueue));
    NodeQueue->next = NULL;
    NodeQueue->x = x;
    sem_init(&NodeQueue->s, 0, 1);

    return NodeQueue;
}

int emptyQueue(struct threadQueue* queue){
    return NULL == queue->head;
}

void insertQueues(struct threadQueue* queue, struct threadNodeQueue* node){
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

struct threadNodeQueue* popQueue(struct threadQueue* queue){
    if(queue->head == NULL)
        return NULL;

    struct threadNodeQueue* temp = queue->head;
    queue->head = temp->next;

    return temp;
}
//------------------------------------------
struct cond{
    sem_t m;
    struct threadQueue *waiters;
};

void wait(struct cond *CV){
    struct threadNodeQueue* self = createNodeQueue();

    sem_wait(&x);
        insertQueues(CV->waiters, self);
    sem_post(&x);

    sem_post(&CV->m);
    sem_wait(&self->s);
    sem_wait(&CV->m);
}

void signal(struct cond* CV){
    sem_wait(&x);
    if(!emptyQueue(CV->waiters)){
        struct threadNodeQueue* temp = popQueue(CV->waiters);
        sem_post(&temp->s);
    }
    sem_post(&x);
}

void brodcast(struct cond* CV){
    sem_wait(&x);

    while(!emptyQueue(CV->waiters)){
        struct threadNodeQueue* temp = popQueue(CV->waiters);
        sem_post(&temp->s);
    }

    sem_post(&x);
}


void* productor(void* arg){
    struct cond *CV = (struct cond*)arg;
    while(1){
        sem_wait(&CV->m);

        while(num > BUFFER_SIZE)
            wait(CV);

        printf("Estoy produciendo\n");

        buffer[add] = (int *)malloc(sizeof(int));
        add = (add + 1) % BUFFER_SIZE;
        num++;

        signal(CV);
        sem_post(&CV->m);

    }
}

void* consumidor(void* arg){
    struct cond *CV = (struct cond*)arg;
    while(1){
        sem_wait(&CV->m);

        while(num == 0)
            wait(CV);

        printf("Estoy consumiendo\n");

        buffer[add] = (int *)malloc(sizeof(int));
        rem = (rem + 1) % BUFFER_SIZE;
        num--;

        signal(CV);
        sem_post(&CV->m);
        
    }
}




int main(){
    sem_init(&x, 0, 1);
    sem_init(&lock, 0, 1);

    struct cond CV;
    CV.waiters = createQueue();
    sem_init(&CV.m, 0, 1);

    //--------------------------
    pthread_t productores[PRODUCTORES];
    pthread_t consumidores[CONSUMIDORES];

    for (int i = 0; i < PRODUCTORES; i++)
        pthread_create(&productores[i], NULL, productor, (void *)&CV);
    for (int i = 0; i < CONSUMIDORES; i++)
        pthread_create(&consumidores[i], NULL, consumidor, (void *)&CV);


    for (int i = 0; i < PRODUCTORES; i++)
        pthread_join(productores[i], NULL);
    for (int i = 0; i < CONSUMIDORES; i++)
        pthread_join(consumidores[i], NULL);


    return 0;
}