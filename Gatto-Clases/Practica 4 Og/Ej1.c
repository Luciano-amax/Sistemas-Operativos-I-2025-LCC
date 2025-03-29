#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define VISITANTES 100000

int Numero[2];
int Eligiendo [2];

void lock(int i){
    /* Calcula el número de turno */
    Eligiendo[i] = 1;
    Numero[i] = 1 + (Numero[0] > Numero[1] ? Numero[0] : Numero[1]);
    asm("mfence");
    Eligiendo[i] = 0;
    asm("mfence");

    /* Compara con todos los hilos */
    for(int j=0; j<2; j++){
        while ( Eligiendo[j] ) { /* busy waiting */ }
        while ((Numero[j] != 0) && ((Numero[j] < Numero[i]) || ((Numero[j] == Numero[i]) && (j < i)))) { /* busy waiting */ }
    }
}

void unlock(int i){
    Numero[i] = 0;
    asm("mfence");
}


int main(){

    int contador = 0;
    #pragma omp parallel shared(contador, Numero, Eligiendo)
    {
        int id = omp_get_thread_num();
        #pragma omp for
        for(int i = 0; i < VISITANTES; i++){
            lock(id);
            contador++;
            unlock(id);
        }

    }
    printf("Cantidad de visitantes %d\n", contador);

    return 0;
}


        

        // #pragma omp for
        // {
        //     for(int i = 0; i < VISITANTES; i++){
        //     lock(omp_get_thread_num());
        //     contador++;
        //     unlock(omp_get_thread_num());
        //     }
        // }
