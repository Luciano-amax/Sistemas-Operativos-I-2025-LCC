#include <stdio.h>
#include <pthread.h>
#include "timing.h"

#define ITERACIONES 500
#define N_PERSONAS 2000000

int suma = 0;

void* molinete() {
    for(int i=0;i<N_PERSONAS;i++)
        suma++;
}

void jardin() {
    molinete();
}

int main() {
    // Supress stderr
    freopen("/dev/null","w",stderr);
    float total = 0;
    for(int i=0;i<ITERACIONES;i++)
    {
        float t;
        TIME_void(jardin(),&t);
        total+=t;
    }
    total/=ITERACIONES;
    printf("Tiempo promedio %f\n", total);
    return 0;
}