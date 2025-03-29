#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>


int main(){
    int n = 4;
    int raiz = (int) floor(sqrt(n));

    int cantDivisores = 0;

        #pragma omp parallel for
        for(int i = 1; i <= raiz; i++)
            #pragma omp critical
            {
            if(n % i == 0)
                cantDivisores++; 
            }

    if(cantDivisores == 1)
    	printf("Es Primo\n");
    else
    	printf("No es primo\n");
    return 0;
}
