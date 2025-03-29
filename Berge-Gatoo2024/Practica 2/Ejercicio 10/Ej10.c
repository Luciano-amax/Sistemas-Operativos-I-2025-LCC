#include <stdio.h>
#include <pthread.h>

#define N 100000LL

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

long long suma = 0;

int arr[N];

void* suma_pares() {
    for(int i=0;i<N;i+=2)
    {
        pthread_mutex_lock(&mutex);
        suma+=arr[i];
        pthread_mutex_unlock(&mutex);
    }
}

void* suma_impares() {
    for(int i=1;i<N;i+=2)
    {
        pthread_mutex_lock(&mutex);
        suma+=arr[i];
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
    // Inicializo el array con los valores desde 1 hasta N
    for(int i=0;i<N;i++)
        arr[i]=i+1;

    // Un hilo sumara los elementos pares y el otro sumara los elementos impares
    pthread_t t1,t2;
    pthread_create(&t1, NULL, suma_pares, NULL);
    pthread_create(&t2, NULL, suma_impares, NULL);

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

    // Verifico si el resultado es correcto
    if(suma == N*(N+1)/2)
        printf("CORRECTO!(%lld)\n",suma);
    else
        printf("INCORRECTO(resultado %lld, esperado %lld)\n", suma, N*(N+1)/2);
}