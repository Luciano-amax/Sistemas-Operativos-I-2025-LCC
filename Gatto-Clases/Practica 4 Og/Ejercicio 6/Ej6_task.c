#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

void swap(int* a, int* b){
    int c = *b;
    *b = *a;
    *a = c;
}

int particionar(int a[], int n){
    int i, j = 0;
    int p = a[0];
    
    swap(&a[0], &a[n-1]);
        for (i = 0; i < n-1; i++) {
            if (a[i] <= p)
                swap(&a[i], &a[j++]);
        }
    
    swap(&a[j], &a[n-1]);
    return j;
}


void quicksort(int a[], int n){
    if (n < 2)
        return;

    int p = particionar(a, n);

    #pragma omp parallel shared(a, n)
    {

        #pragma omp task
        {
            quicksort(a, p);
        }

        #pragma omp task
        {
            quicksort(a + p +1, n -p -1);
        }
    
    }
}



int main(){
    int a[] = {5,4,6,3,8,2,1,7,9};
    int n = 9;

    quicksort(a, n);

    for(int i = 0; i < 9; i++)
        printf("%d ", a[i]);
    printf("\n");

    return 0;
}