#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LIMITE 100

int total = 0;
int posicion = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* thread(void* arg){
    int* arregloCompartido = (int *) arg;
    
    while(posicion < LIMITE){
        pthread_mutex_lock(&mutex);
        total = total + arregloCompartido[posicion];
        posicion++;
        pthread_mutex_unlock(&mutex);
    }
}

int main(){
    int arregloCompartido[LIMITE];
    
    for(int i = 0; i < LIMITE; i++)
        arregloCompartido[i] = i + 1;
    
    pthread_t id1, id2;
    pthread_create(&id1, NULL, &thread, (void *)arregloCompartido);
    pthread_create(&id2, NULL, &thread, (void *)arregloCompartido);

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);

    printf("Contador: %d\n", total);

    return 0;
}
