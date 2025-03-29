#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define VISITANTES 10000
int contador = 0;

void* molinete1(void* arg){
    for(int i = 0; i < VISITANTES; i++)
        contador++;
}

void* molinete2(void* arg){
    for(int i = 0; i < VISITANTES; i++)
        contador++;
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