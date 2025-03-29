#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 45

#define PRODUCTORES 10
#define CONSUMIDORES 8

sem_t sem_lugares, sem_ocupados, sem_mutex;

int* buffer[BUFFER_SIZE];
int add = 0, rem = 0;

void* productor(){
    while(1){
        sem_wait(&sem_lugares);
        
        sem_wait(&sem_mutex);
        printf("Estoy produciendo\n");
        sleep(1);

        buffer[add] = (int *)malloc(sizeof(int));
        add = (add + 1) % BUFFER_SIZE;

        sem_post(&sem_mutex);

        sem_post(&sem_ocupados);
    }
}

void* consumidor(){
    while(1){

        sem_wait(&sem_ocupados);
        
        sem_wait(&sem_mutex);
        printf("Estoy consumiendo\n");
        sleep(1);


        free(buffer[rem]);
        rem = (rem + 1) % BUFFER_SIZE;

        sem_post(&sem_mutex);
        sem_post(&sem_lugares);
    }
}

int main (){
    pthread_t productores[PRODUCTORES];
    pthread_t consumidores[CONSUMIDORES];
    
    sem_init(&sem_lugares, 0, BUFFER_SIZE);
    sem_init(&sem_ocupados, 0, 0);     // 0 por usar pthread
    sem_init(&sem_mutex, 0, 1);

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