#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <semaphore.h>
sem_t fumador_1, fumador_2, fumador_3, otra_vez;

void agente(){
    while (1) {
        sem_wait(&otra_vez);
        int caso = random() % 3;
        if (caso == 0) sem_post(&fumador_1);
        if (caso == 1) sem_post(&fumador_2);
        if (caso == 2) sem_post(&fumador_3);
    }
}

void fumar(int fumador){
    printf("Fumador %d: Puf! Puf! Puf!\n", fumador);
    sleep(1);
}

void * fumador1(void *arg){
    while (1) {
            sem_wait(&fumador_1);
            sem_post(&otra_vez);
            fumar(1);
    }
}

void * fumador2(void *arg){
    while (1) {
            sem_wait(&fumador_2);
            sem_post(&otra_vez);
            fumar(2);
    }
}

void * fumador3(void *arg){
    while (1) {
            sem_wait(&fumador_3);
            sem_post(&otra_vez);
            fumar(3);
    }
}

int main(){
pthread_t s1, s2, s3;

sem_init(&fumador_1, 0, 0);
sem_init(&fumador_2, 0, 0);
sem_init(&fumador_3, 0, 0);
sem_init(&otra_vez, 0, 1);

pthread_create(&s1, NULL, fumador1, NULL);
pthread_create(&s2, NULL, fumador2, NULL);
pthread_create(&s3, NULL, fumador3, NULL);
agente();
return 0;
}