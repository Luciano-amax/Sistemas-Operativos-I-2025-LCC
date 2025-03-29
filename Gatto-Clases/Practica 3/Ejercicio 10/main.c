#include <stdio.h>
#include <pthread.h>
#include "barrier.h"

#define N 10
#define ITERS 20
#define W 5

struct barrier b;

float arr1[N], arr2[N];
void calor(float *arr, int lo, int hi, float *arr2) {
    int i;
    for (i = lo; i < hi; i++) {
        int m = arr[i];
        int l = i > 0 ? arr[i-1] : m;
        int r = i < N-1 ? arr[i+1] : m;
        arr2[i] = m + (l - m)/1000.0 + (r - m)/1000.0;
    }
}

static inline int min(int a,int b) {
    return (a<b) ? a : b;
}

/* Dado un array de [n], devuelve el punto de corte [i] de los [m] totales. */
static inline int cut(int n, int i, int m) {
    return i * (n/m) + min(i, n %m);
}

void * thr(void *arg) {
    int id = arg - (void*)0; /* 0 <= id < W */
    int lo = cut(N, id, W), hi = cut(N, id+1, W);
    int i;
    for (i = 0; i < ITERS; i++) {
        calor(arr1, lo, hi, arr2);
        calor(arr2, lo, hi, arr1);
        barrier_wait(&b);
    }
}

void print_arr(float* arr, int n) {
    for(int i=0;i<n;i++)
        printf("%.2f ", arr[i]);
}

int main() {
    pthread_t id[W];
    
    // Valores de ejemplo para los arrays
    for(int i=0;i<N;i++)
        arr1[i] = i+1;
    for(int i=0;i<N;i++)
        arr2[i] = N-i;

    // Muestro el valor inicial
    printf("MAT1: ");
    print_arr(arr1, N);
    printf("\nMAT2: ");
    print_arr(arr2, N);
    putchar('\n');

    // Ejecuto los threads
    barrier_init(&b, W);
    for(int i=0;i<W;i++)
        pthread_create(id+i, NULL, thr, (void*)i);
    for(int i=0;i<W;i++)
        pthread_join(id[i], NULL);

    // Muestro el valor final
    printf("MAT1: ");
    print_arr(arr1, N);
    printf("\nMAT2: ");
    print_arr(arr2, N);
    putchar('\n');
    return 0;
}
