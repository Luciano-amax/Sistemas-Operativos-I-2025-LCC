#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define M 5
#define N 5
#define ARRLEN 10240

pthread_mutex_t mutex =  PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lector =  PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_escritor =  PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_justo =  PTHREAD_MUTEX_INITIALIZER;

int cantLectores = 0;
int cantEscritores = 0;

int arr[ARRLEN];

void * escritor(void *arg){
    int i;
    int num = arg - (void*)0;
    while (1) {

        pthread_mutex_lock(&mutex_justo);
        pthread_mutex_lock(&mutex);
        pthread_mutex_unlock(&mutex_justo);
        
 
        sleep(random() % 3);
        printf("Escritor %d escribiendo\n", num);
        for (i = 0; i < ARRLEN; i++)
            arr[i] = num;
        pthread_mutex_unlock(&mutex);

    }
    return NULL;
}

void * lector(void *arg){
    int v, i;
    int num = arg - (void*)0;
    while (1) {

        pthread_mutex_lock(&mutex_justo);

        pthread_mutex_lock(&mutex_lector);
        cantLectores++;
        if(cantLectores == 1)
            pthread_mutex_lock(&mutex);

        pthread_mutex_unlock(&mutex_justo);
        pthread_mutex_unlock(&mutex_lector);


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


        pthread_mutex_lock(&mutex_lector);
        cantLectores--;
        if(cantLectores == 0)
            pthread_mutex_unlock(&mutex);
        pthread_mutex_unlock(&mutex_lector);
    }

    return NULL;
}

int main(){
    pthread_t lectores[M], escritores[N];
    int i;
    for (i = 0; i < M; i++)
        pthread_create(&lectores[i], NULL, lector, i + (void*)0);
    for (i = 0; i < N; i++)
        pthread_create(&escritores[i], NULL, escritor, i + (void*)0);
  
            
    pthread_join(lectores[0], NULL); /* Espera para siempre */
    return 0;
}

/*
    Los eescritores pueden sufrir stravation porque puede pasar que los lectores nunca dejen
    de leer, haciendo que nunca se pueda escribir.
*/