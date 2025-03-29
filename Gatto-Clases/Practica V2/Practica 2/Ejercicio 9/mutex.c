#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define VISITANTES 10000
int contador = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* molinete1(void* arg){
    for(int i = 0; i < VISITANTES; i++){
        pthread_mutex_lock(&mutex);
        contador++;
        pthread_mutex_unlock(&mutex);
    }
}

void* molinete2(void* arg){
    for(int i = 0; i < VISITANTES; i++){
        pthread_mutex_lock(&mutex);
        contador++;
        pthread_mutex_unlock(&mutex);
    }
}

int main(){
    pthread_t id1, id2;
    pthread_create(&id1, NULL, molinete1, NULL);
    pthread_create(&id2, NULL, molinete2, NULL);

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);

    printf("Visitantes Totales: %d\n", contador);

    return 0;
}