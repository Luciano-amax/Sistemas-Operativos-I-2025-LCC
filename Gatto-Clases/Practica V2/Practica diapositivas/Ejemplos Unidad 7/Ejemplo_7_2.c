#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 10
#define PRODUCTORES_CANT 5
#define CONSUMIDORES_CANT 5

sem_t mutex, lugar, ocupado;

int buffer[BUFFER_SIZE];
int num = 0, posProduc = 0, posConsu = 0;

void* productor(void *arg){
    int id = *((int *) arg) + 1;
    while(1){
        sem_wait(&lugar);

        sem_wait(&mutex);
        printf("El productor %d esta produciendo\n", id);
        sleep(1);
        buffer[posProduc] = 1;
        posProduc = (posProduc + 1) % BUFFER_SIZE;
        num++;
        sem_post(&mutex);

        sem_post(&ocupado);
    }
}

void* consumidor(void* arg){
    int id = *((int *) arg) + 1;
    while(1){
        sem_wait(&ocupado);

        sem_wait(&mutex);
        printf("El consumidor %d esta consumiendo\n", id);
        sleep(1);
        buffer[posConsu] = 0;
        posConsu = (posConsu + 1) % BUFFER_SIZE;
        num--;
        sem_post(&mutex);

        sem_post(&lugar);
    }
}


int main(){
    sem_init(&mutex, 0, 1);
    sem_init(&lugar, 0, BUFFER_SIZE);
    sem_init(&ocupado, 0, 0);

    pthread_t productores[PRODUCTORES_CANT], consumidores[CONSUMIDORES_CANT]; 
    int idProduc[PRODUCTORES_CANT], idConsu[CONSUMIDORES_CANT];

    for(int i = 0; i < PRODUCTORES_CANT; i++){
        idProduc[i] = i;
        pthread_create(&productores[i], NULL, productor, &idProduc[i]);
    }
    for(int i = 0; i < CONSUMIDORES_CANT; i++){
        idConsu[i] = i;
        pthread_create(&consumidores[i], NULL, consumidor, &idProduc[i]);
    }

    for(int i = 0; i < PRODUCTORES_CANT; i++)
        pthread_join(productores[i], NULL);
    for(int i = 0; i < CONSUMIDORES_CANT; i++)
        pthread_join(consumidores[i], NULL);

    return 0;
}