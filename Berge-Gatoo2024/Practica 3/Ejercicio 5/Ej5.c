#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "rw_lock_justo.c"
#define M 1000
#define N 1000
#define ARRLEN 10240
#define ESPERA 3

rwlock_t lock;

int arr[ARRLEN];

void * escritor(void *arg){
    int i;
    int num = arg - (void*)0;
    while (1) {
        rwlock_wrlock(&lock);
        sleep(random() % ESPERA);

        printf("Escritor %d escribiendo\n", num);
        for (i = 0; i < ARRLEN; i++)
            arr[i] = num;
        rwlock_unlock(&lock);
    }
    return NULL;
}

void * lector(void *arg){
    int v, i;
    int num = arg - (void*)0;
    while (1) {
        rwlock_rdlock(&lock);
        sleep(random() % ESPERA);
        v = arr[0];
        for (i = 1; i < ARRLEN; i++) {
            if (arr[i] != v)
            break;
        }
        if (i < ARRLEN)
            printf("Lector %d, error de lectura\n", num);
        else
            printf("Lector %d, dato %d\n", num, v);

        rwlock_unlock(&lock);
    }

    return NULL;
}

int main(){
    rwlock_init(&lock);
    pthread_t lectores[M], escritores[N];
    int i;

    int maximo = N > M ? N : M;
    
    for (i = 0; i < maximo; i++)
    {
        if(i<M)   
            pthread_create(&lectores[i], NULL, lector, i + (void*)0);
        
        if(i<N)
            pthread_create(&escritores[i], NULL, escritor, i + (void*)0);
    }
            
    pthread_join(lectores[0], NULL); /* Espera para siempre */
    return 0;
}
