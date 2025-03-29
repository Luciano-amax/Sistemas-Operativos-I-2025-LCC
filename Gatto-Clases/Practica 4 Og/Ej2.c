#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define N 8

void ponerUnos(double arreglo[]){
    for(int i = 0; i < N; i++)
        arreglo[i] = 1;
}

int main(){
      
    double suma = 0;
    double* arreglo = malloc(sizeof(double) * N);
    ponerUnos(arreglo);

    #pragma omp parallel shared(suma)
    {
        #pragma omp for reduction(+: suma)
        for(int i = 0; i < N; i++){
            suma = suma + arreglo[i];
        }

    }

    printf("Suma total %.2f\n", suma);

    return 0;
}

/*
int main(){
      
    double suma = 0;
    double* arreglo = malloc(sizeof(double) * N);
    ponerUnos(arreglo);

    #pragma omp parallel for reduction(+: suma)
    for(int i = 0; i < N; i++){
    
        suma = suma + arreglo[i];
    }

    printf("Suma total %.2f\n", suma);

    return 0;
}
*/
