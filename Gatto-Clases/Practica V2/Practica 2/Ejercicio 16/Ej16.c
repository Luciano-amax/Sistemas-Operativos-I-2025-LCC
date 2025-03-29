#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define NUMERO_PROCESOS 2
#define VISITANTES 10000

int numero[NUMERO_PROCESOS];
int eligiendo[NUMERO_PROCESOS];
int contador = 0;

void lock(int idProceso){
    
    eligiendo[idProceso] = 1;
    
    for(int i = 0; i < NUMERO_PROCESOS; i++){
        if(numero[idProceso] < numero[i])
            numero[idProceso] = numero[i];
    }
    asm("mfence");
    numero[idProceso]++;
    asm("mfence");
    
    eligiendo[idProceso] = 0;

    for(int j = 0; j < NUMERO_PROCESOS; j++){
        while (eligiendo[j]);
        while ((numero[j] != 0) && ((numero[j] < numero[idProceso]) || ((numero[j] == numero[idProceso]) && (j < idProceso))));
    }
}

void unlock(int idProceso){
    numero[idProceso] = 0;
    asm("mfence");
}

void* molinete(void* arg){
    int id = *((int*)arg);
    for(int i = 0; i < VISITANTES; i++){
        lock(id);
        contador++;
        unlock(id);
    }
}

int main(){
    pthread_t t[NUMERO_PROCESOS];
    int id[NUMERO_PROCESOS];

    for(int i = 0; i < NUMERO_PROCESOS; i++){
        id[i] = i;
        pthread_create(&t[i], NULL, molinete, id+i);
        pthread_join(t[i], NULL);
    }

    printf("Visitantes: %d\n", contador);

    return 0;
}
