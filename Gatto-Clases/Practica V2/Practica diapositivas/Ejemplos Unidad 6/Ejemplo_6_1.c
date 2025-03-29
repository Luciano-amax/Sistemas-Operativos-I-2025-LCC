#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

int contador = 0;
void* thread1(void* arg){
    for(int i = 0; i < 10000; i++){
    pthread_mutex_lock(&mutex1);
    pthread_mutex_lock(&mutex2);

    contador++;

    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    }
}

void* thread2(void* arg){
    for(int i = 0; i < 10000; i++){
    pthread_mutex_lock(&mutex2);
    pthread_mutex_lock(&mutex1);

    contador++;

    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    }
}

int main(){
    pthread_t id1, id2;
    pthread_create(&id1, NULL, thread1, NULL);
    pthread_create(&id2, NULL, thread2, NULL);

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);

    printf("Contador %d\n", contador);

    return 0;
}