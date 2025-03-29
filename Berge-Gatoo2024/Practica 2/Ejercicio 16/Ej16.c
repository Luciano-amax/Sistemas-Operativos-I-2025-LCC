#include <stdio.h>
#include <pthread.h>
#include "timing.h"

#define ITERACIONES 500
#define N_MOLINENTES 2
#define N_PERSONAS 1000000

int suma = 0;

int numero[N_MOLINENTES];
int bandera[N_MOLINENTES];
int turno = 0;

void lock(int id) {
    bandera[id]=1;
    // Agarra el maximo
    numero[id]=-1;
    for(int i=0;i<N_MOLINENTES;i++)
    {
        if(numero[i]>numero[id])
            numero[id]=numero[i];
    }
    numero[id]++;
    bandera[id]=0;

    for(int j=0;j<N_MOLINENTES;j++)
    {
        while(bandera[j]);
        while ((numero[j] != 0) &&((numero[j] < numero[id]) || ((numero[j] == numero[id]) && (j < id))));
    }
}

void unlock(int id) {
    numero[id] = 0;
}

void* molinete(void* ptr_id) {
    int id = *((int*)ptr_id);
    for(int i=0;i<N_PERSONAS;i++)
    {
        lock(id);
        suma++;
        unlock(id);
    }
}

void jardin(){
    pthread_t t[N_MOLINENTES];
    int id[N_MOLINENTES];
    for(int i=0;i<N_MOLINENTES;i++){   
        id[i]=i;
        pthread_create(t+i,NULL,molinete,id+i);
        pthread_join(t[i],NULL);
    }

    for(int i=0;i<N_MOLINENTES;i++)
        pthread_join(t[i], NULL);
}

int main() {
    freopen("/dev/null","w",stderr);
    float total = 0;

    for(int i=0;i<ITERACIONES;i++){
        float t;
        TIME_void(jardin(),&t);
        total+=t;
    }
        
    total/=ITERACIONES;
    printf("Tiempo promedio %f\n", total);
    return 0;
}

