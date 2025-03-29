#include <stdio.h>
#include <pthread.h>
#include "timing.h"

#define ITERACIONES 500
#define N_PERSONAS 1000000

int suma = 0;

//Mutex
pthread_mutex_t mutex_visitantes = PTHREAD_MUTEX_INITIALIZER;

void* molinete(void* ptr_id) {
    int id = *((int*)ptr_id);
    for(int i=0;i<N_PERSONAS;i++)
    {
        pthread_mutex_lock(&mutex_visitantes);
        suma++;
        pthread_mutex_unlock(&mutex_visitantes);
    }
    pthread_exit(NULL);
}

void jardin() {
    suma = 0;
    pthread_t t1,t2;
    int id1 = 0, id2 = 1;
    pthread_create(&t1, NULL, molinete, &id1);
    pthread_create(&t2, NULL, molinete, &id2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}

int main() {
    // Supress stderr
    freopen("/dev/null","w",stderr);
    float total = 0;
    for(int i=0;i<ITERACIONES;i++)
    {
        float t;
        TIME_void(jardin(),&t);
        total+=t;
    }
    total/=ITERACIONES;
    printf("Tiempo promedio %f\n", total);
    return 0;
}