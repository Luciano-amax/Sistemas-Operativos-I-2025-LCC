#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 45

#define PRODUCTORES 10
#define CONSUMIDORES 8

pthread_mutex_t mutex_buffer =  PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t non_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t non_empty = PTHREAD_COND_INITIALIZER;

int* buffer[BUFFER_SIZE];
int num = 0, add = 0, rem = 0;

void* productor(){
    while(1){
        pthread_mutex_lock(&mutex_buffer);

        while(num > BUFFER_SIZE)
            pthread_cond_wait(&non_full, &mutex_buffer);

        printf("Estoy produciendo\n");

        buffer[add] = (int *)malloc(sizeof(int));
        add = (add + 1) % BUFFER_SIZE;
        num++;

        pthread_cond_signal(&non_empty);
        pthread_mutex_unlock(&mutex_buffer);

    }
}

void* consumidor(){
    while(1){
        pthread_mutex_lock(&mutex_buffer);

        while(num == 0)
            pthread_cond_wait(&non_empty, &mutex_buffer);

        printf("Estoy consumiendo\n");

        buffer[add] = (int *)malloc(sizeof(int));
        rem = (rem + 1) % BUFFER_SIZE;
        num--;

        pthread_cond_signal(&non_full);
        pthread_mutex_unlock(&mutex_buffer);
        
    }
}


int main (){
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

    printf("Dsdsd");

    return 0;
}