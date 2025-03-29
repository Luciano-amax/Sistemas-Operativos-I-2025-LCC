#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define VISITANTES 10000
int contador = 0;

void* molinete(void* arg){
    for(int i = 0; i < VISITANTES; i++)
        contador++;
}

int main(){
    molinete(NULL);
    molinete(NULL);
    
    printf("Visitantes Totales: %d\n", contador);

    return 0;
}
