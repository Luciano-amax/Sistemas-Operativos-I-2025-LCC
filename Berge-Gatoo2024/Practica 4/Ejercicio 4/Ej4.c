#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include "timing.h"

int esPrimo(const long n) {
    if(n==2 || n==3)
        return 1;
    if(n<= 1 || n % 2 == 0 || n%3 == 0)
        return 0;
    
    const long raiz = (long) sqrt(n);

    int primo = 1;

    #pragma omp parallel shared(n, primo, raiz)
    {
        #pragma omp for
        // Todo numero primo >=3 es de la forma 6k+1 o 6k+5 con k un numero natural
        // Esto permite reducir el numero de divisores a testear a un tercio
        for(long i = 5; i <= raiz; i+=6)
            // i es de la forma 6k+5, i+2 es de la forma 6k+1
            if(n % i == 0 || n % (i+2) == 0)
                primo = 0;
    }

    return primo;
}


int test(){
    // Testeo los numeros primos
    int fails=0;
    int total=0;

    
    freopen("primos.txt", "r", stdin);
    for(long n; scanf("%ld", &n) != EOF;total++)
        if(!esPrimo(n))
        {
            printf("Error en test %d: %ld marcado como no primo cuando es primo\n", total+1, n);
            fails++;
        }

    freopen("no-primos.txt", "r", stdin);
    for(long n; scanf("%ld", &n) != EOF;total++)
        if(esPrimo(n))
        {
            printf("Error en test %d: %ld marcado como primo cuando no es primo\n", total+1, n);
            fails++;
        }
    
    printf("%d/%d test superados, %d/%d test fallados\n", (total-fails),total, fails,total);

    return 0;
}

int main() {
    TIME_void(test(), NULL);
}
