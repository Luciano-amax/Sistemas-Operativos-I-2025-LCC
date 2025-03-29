#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define N 5000

void ponerUnos(double arreglo[]){
    for(int i = 0; i < N; i++)
        arreglo[i] = 1;
}

int main(){ 
    double* arreglo = malloc(sizeof(double) * N);
    ponerUnos(arreglo);
    arreglo[50] = -1;
    double menor = arreglo[0];
    #pragma omp parallel shared(menor)
    {
        #pragma omp for
        for(int i = 0; i < N; i++){
            #pragma omp critical
            {
            menor = (menor > arreglo[i] ? arreglo[i] : menor);
            }
        }

    }

    printf("El menor es %.2f\n", menor);

    return 0;
}

/*
Otra Forma

int main(){ 
    double* arreglo = malloc(sizeof(double) * N);
    ponerUnos(arreglo);
    arreglo[50] = -1;
    double menor = arreglo[0];
    
    #pragma omp parallel for
    for(int i = 0; i < N; i++){
        #pragma omp critical
        {
        menor = (menor > arreglo[i] ? arreglo[i] : menor);
        }
    }

    printf("El menor es %.2f\n", menor);

    return 0;
}

*/
