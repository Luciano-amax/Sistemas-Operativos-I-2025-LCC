#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 10

#define PRODUCTORES 5
#define CONSUMIDORES 5

sem_t lock, s, x, h;
int threadCount = 0;

int* buffer[BUFFER_SIZE];
int num = 0, add = 0, rem = 0;

void wait(){
    sem_wait(&x);
    threadCount++;
    sem_post(&x);

    sem_post(&lock);

    sem_wait(&s);
    sem_post(&h);

    sem_wait(&lock);
}

void signal(){
    sem_wait(&x);

    if(threadCount > 0){
        threadCount--;
        sem_post(&s);
        sem_wait(&h);
    }

    sem_post(&x);
}

void brodcast(){
    sem_wait(&x);

    for(int i = 0; i < threadCount; i++)
        sem_post(&s);

    while(threadCount > 0){
        threadCount--;
        sem_wait(&h);
    }

    sem_post(&x);    
}


void* productor(){
    while(1){
        sem_wait(&lock);

        while(num >= BUFFER_SIZE)
            wait();

        printf("Estoy produciendo\n");

        buffer[add] = (int *)malloc(sizeof(int));
        add = (add + 1) % BUFFER_SIZE;
        num++;

        signal();
        sem_post(&lock);

    }
}

void* consumidor(){
    while(1){
        sem_wait(&lock);

        while(num == 0)
            wait();

        printf("Estoy consumiendo\n");

        buffer[add] = (int *)malloc(sizeof(int));
        rem = (rem + 1) % BUFFER_SIZE;
        num--;

        signal();
        sem_post(&lock);
        
    }
}


int main(){
    //Creo los semaforos para la CV
    sem_init(&s, 0, 0);
    sem_init(&x, 0, 1);
    sem_init(&h, 0, 0);
    sem_init(&lock, 0, 1);

    pthread_t productores[PRODUCTORES];
    pthread_t consumidores[CONSUMIDORES];

    for (int i = 0; i < PRODUCTORES; i++)
        pthread_create(&productores[i], NULL, productor, NULL);
    for (int i = 0; i < CONSUMIDORES; i++)
        pthread_create(&consumidores[i], NULL, consumidor, NULL);


    for (int i = 0; i < PRODUCTORES; i++)
        pthread_join(productores[i], NULL);
    for (int i = 0; i < CONSUMIDORES; i++)
        pthread_join(consumidores[i], NULL);


    return 0;
}