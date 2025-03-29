#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define M 5
#define N 5
#define ARRLEN 10240

sem_t turnoLectores, turno, justo;

int cantLectores = 0;
int cantEscritores = 0;

int arr[ARRLEN];

void * escritor(void *arg){
    int i;
    int num = arg - (void*)0;
    while (1) {

        sem_wait(&justo);
        sem_wait(&turno);
        sem_post(&justo);
        
 
        sleep(random() % 3);
        printf("Escritor %d escribiendo\n", num);
        for (i = 0; i < ARRLEN; i++)
            arr[i] = num;

        sem_post(&turno);

    }
    return NULL;
}

void * lector(void *arg){
    int v, i;
    int num = arg - (void*)0;
    while (1) {

        sem_wait(&justo);
        sem_wait(&turnoLectores);

        cantLectores++;
        if(cantLectores == 1)
            sem_wait(&turno);

        sem_post(&justo);
        sem_post(&turnoLectores);


        sleep(random() % 3);
        v = arr[0];
        for (i = 1; i < ARRLEN; i++) {
            if (arr[i] != v)
            break;
        }
        if (i < ARRLEN)
            printf("Lector %d, error de lectura\n", num);
        else
            printf("Lector %d, dato %d\n", num, v);


        sem_wait(&turnoLectores);
        cantLectores--;
        if(cantLectores == 0)
            sem_post(&turno);
        sem_post(&turnoLectores);
    }

    return NULL;
}

int main(){
    pthread_t lectores[M], escritores[N];
    sem_init(&turnoLectores, 0, 1);
    sem_init(&turno, 0, 1);
    sem_init(&justo, 0, 1);

    int i;
    for (i = 0; i < M; i++)
        pthread_create(&lectores[i], NULL, lector, i + (void*)0);
    for (i = 0; i < N; i++)
        pthread_create(&escritores[i], NULL, escritor, i + (void*)0);
  
            
    pthread_join(lectores[0], NULL); /* Espera para siempre */
    return 0;
}
