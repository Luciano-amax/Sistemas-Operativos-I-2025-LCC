#include <stdio.h>
#include <pthread.h>
#define N_PERSONAS 1000000

int suma = 0;

int bandera[2];
int turno = 0;

void lock(int id) {
    int id2 = (id+1)%2;
    bandera[id] = 1;
    turno = id2;
    asm("mfence");
    while(bandera[id2] && turno == id2);
}

void unlock(int id) {
    bandera[id] = 0;
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

int main() {
    pthread_t t1,t2;
    int id1 = 0, id2 = 1;
    pthread_create(&t1, NULL, molinete, &id1);
    pthread_create(&t2, NULL, molinete, &id2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Entraron %d personas\n",suma);
    return 0;
}