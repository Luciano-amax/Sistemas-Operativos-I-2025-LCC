#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define VISITANTES 10000

static inline void incl(int *p){
    asm("lock; incl %0": "+m"(*p)::"memory");	
}


int contador = 0;
int flag[2];
int turno;

void* molinete1(void* arg){
    for(int i = 0; i < VISITANTES; i++){
        incl(&contador);
    }
}

void* molinete2(void* arg){
    for(int i = 0; i < VISITANTES; i++){
        incl(&contador);
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
