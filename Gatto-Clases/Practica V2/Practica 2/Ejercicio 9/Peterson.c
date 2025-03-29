#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define VISITANTES 10000


int contador = 0;
int flag[2];
int turno;

void* molinete1(void* arg){
    for(int i = 0; i < VISITANTES; i++){
        flag[0] = 1;
        turno = 1;
        asm("mfence");
        while(flag[1] && (turno == 1));

        contador++;

        flag[0] = 0;
    }
}

void* molinete2(void* arg){
    for(int i = 0; i < VISITANTES; i++){
        flag[1] = 1;
        turno = 0;
        asm("mfence");
        while(flag[0] && (turno == 0));

        contador++;

        flag[1] = 0;
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