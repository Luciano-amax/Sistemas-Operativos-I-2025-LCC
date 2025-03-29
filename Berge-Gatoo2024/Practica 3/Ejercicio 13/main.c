#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "cond.h"

#define BUFFER_SIZE 10

#define PRODUCTORES 5
#define CONSUMIDORES 5

int* buffer[BUFFER_SIZE];
int num = 0, add = 0, rem = 0;

//-----------------------

void* productor(void* arg){
    struct cond *CV = (struct cond*)arg;
    while(1){
        sem_wait(&CV->m);

        while(num >= BUFFER_SIZE)
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
    struct cond CV;
    CV.waiters = create_queue();
    sem_init(&CV.x, 0, 1);
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
