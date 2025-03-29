#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>

#define N_SILLAS 4
#define TOTAL 50000

int cantClienetes = 0;
sem_t mutex, cliente_sem, barbero_sem, clienteTermino, barberoTermino;

void* cliente(void* arg){
    
    sem_wait(&mutex);
        if(cantClienetes == N_SILLAS){
            sem_post(&mutex);
            pthread_exit(0);        
        }
        else if(cantClienetes == 0)
            printf("Despierto al barbero\n");
        cantClienetes++;
    sem_post(&mutex);

    sem_post(&cliente_sem);
    sem_wait(&barbero_sem);
    printf("Barbero me esta cortando el pelo\n");

    sem_post(&clienteTermino);
    sem_wait(&barberoTermino);

    sem_wait(&mutex);
    cantClienetes--;
    sem_post(&mutex);
}

void* barbero(void* arg){
    while(1){
        printf("Espero al siguiente cliente\n");
        sem_wait(&cliente_sem);
        sem_post(&barbero_sem);

        printf("Le corto el pelo al cliente\n");

        sem_wait(&clienteTermino);
        sem_post(&barberoTermino);
    }
}


int main(){
    sem_init(&mutex, 0, 1);
    sem_init(&cliente_sem, 0, 0);
    sem_init(&barbero_sem, 0, 0);
    sem_init(&clienteTermino, 0, 0);
    sem_init(&barberoTermino, 0, 0);

    pthread_t barbero_, clientes[TOTAL];

    pthread_create(&barbero_, NULL, barbero, NULL);

    for(int i = 0; i < TOTAL; i++)
        pthread_create(&clientes[i], NULL, cliente, NULL);

    pthread_join(clientes[0], NULL);
    return 0;
}