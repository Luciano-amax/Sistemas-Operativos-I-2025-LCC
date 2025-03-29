#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define VISITANTES 1000

int visitantes = 0;

void* molinete(void* arg){
    int old, new;

    for(int i = 0; i < VISITANTES; i++){
        do{
            old = visitantes;
            new = old +1;
        }while(!casX86(&visitantes, old, new));
    }
}

int main(){
    pthread_t id1, id2;
    pthread_create(&id1, NULL, molinete, NULL);
    pthread_create(&id2, NULL, molinete, NULL);

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);

    printf("Visitantes: %d\n", visitantes);

    return 0;
}