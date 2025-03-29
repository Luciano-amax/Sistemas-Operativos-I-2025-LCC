#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct argumentos{
    int* a;
    int n;
};

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


void quicksort(void* datos){
    struct argumentos* arg = (struct argumentos*) datos;
    if (arg->n < 2)
        return;

    int p = particionar(arg->a, arg->n);

    pthread_t id1, id2;
    struct argumentos arg1 = {(arg->a),p};
    struct argumentos arg2 = {(arg->a) + p + 1, (arg->n) - p - 1};

    pthread_create(&id1, NULL, (void*)&quicksort, (void*) (&arg1));
    pthread_create(&id2, NULL, (void*)&quicksort, (void*) (&arg2));
    
    pthread_join(id1, NULL);
    pthread_join(id2, NULL);
}

int main(){
    int a[] = {5,4,6,3,8,2,1,7,9};

    struct argumentos arg1;
    arg1.a = a;
    arg1.n = 9;

    quicksort((void*) (&arg1));

    for(int i = 0; i < 9; i++)
        printf("%d ", a[i]);
    printf("\n");

    return 0;
}