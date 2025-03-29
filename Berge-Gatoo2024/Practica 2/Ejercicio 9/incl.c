#include <stdio.h>
#include <pthread.h>
#include "timing.h"

#define ITERACIONES 500
#define N_PERSONAS 1000000

int suma = 0;

static inline void incl(int *p) {
    asm("lock; incl %0" : "+m"(*p) : : "memory");
}

void* molinete(void* ptr_id) {
    int id = *((int*)ptr_id);
    for(int i=0;i<N_PERSONAS;i++)
        incl(&suma);
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
        TIME_void(jardin_incl(),&t);
        total+=t;
    }
    total/=ITERACIONES;
    printf("Tiempo promedio %f\n", total);
    return 0;
}