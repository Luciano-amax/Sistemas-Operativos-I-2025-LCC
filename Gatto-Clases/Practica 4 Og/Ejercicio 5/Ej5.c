#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

int main(){
    int N=3;

    int matrizA[3][3] = {{1,2,3},{4,5,6},{7,8,9}};
    int matrizB[3][3] = {{2,0,0},{0,2,0},{0,0,2}};
    int matrizC[3][3];

    	#pragma omp parallel for
        for(int i = 0; i < N; i++){
            #pragma omp parallel for
            for(int j = 0; j < N; j++){
                #pragma omp parallel for 
                for(int k = 0; k < N; k++){
                    #pragma omp critical
                    {
                        matrizC[i][j] += matrizA[i][k] * matrizB[k][j];
                    }
                }
            }
        }

    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++)
            printf("%d ", matrizC[i][j]);
        printf("\n");
    }

    return 0;
}
/*
#pragma omp parallel for
        for(int i = 0; i < N; i++){
            #pragma omp parallel for
            for(int j = 0; j < N; j++){
                #pragma omp parallel for
                for(int k = 0; k < N; k++){
                    #pragma omp critical
                    {
                        matrizC[i][j] += matrizA[i][k] * matrizB[k][j];
                    }
                }
            }
        }


#pragma omp parallel shared(matrizA, matrizB, matrizC)
    {
        #pragma omp for
        for(int i = 0; i < N; i++){
            #pragma omp parallel shared(matrizA, matrizB, matrizC,i)
            {   
            #pragma omp for
            for(int j = 0; j < N; j++){
                #pragma omp parallel shared(matrizA, matrizB, matrizC,i,j)
                {
                #pragma omp for
                for(int k = 0; k < N; k++){
                    #pragma omp critical
                    {
                        matrizC[i][j] += matrizA[i][k] * matrizB[k][j];
                    }
                }
                }
            }
            }
        }
    }

*/
