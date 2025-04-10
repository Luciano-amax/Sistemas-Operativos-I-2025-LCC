#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include <pthread.h>

int visitantes = 0;

#define NUM_VISITANTES 40

pthread_mutex_t mutex_resources1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_resources2 = PTHREAD_MUTEX_INITIALIZER;


void* thread1(void* v){
    
    pthread_mutex_lock(&mutex_resources1);

    printf("Threead A mutex_resource1\n");

    sleep(1);

    printf("Threead A waiting mutex_resource2\n");

    if(pthread_mutex_trylock(&mutex_resources2) == 16){
        pthread_mutex_unlock(&mutex_resources1);
    }


    pthread_mutex_lock(&mutex_resources2);
    
    
    printf("Region critica Thread A\n");
    pthread_mutex_unlock(&mutex_resources2);
    pthread_mutex_unlock(&mutex_resources1);
        
}
void* thread2(void* v){
    
    pthread_mutex_lock(&mutex_resources2);

    printf("Threead B mutex_resource2\n");

    sleep(1);

    printf("Threead B waiting mutex_resource1\n");

    if(pthread_mutex_trylock(&mutex_resources1) == 16){
        pthread_mutex_unlock(&mutex_resources2);
    }
    
        
    printf("Region critica Thread B\n");
    pthread_mutex_unlock(&mutex_resources1);
    pthread_mutex_unlock(&mutex_resources2);
        
}

int main(){
    pthread_t m1, m2;

    pthread_create(&m1, NULL ,thread1,NULL);
    pthread_create(&m2, NULL ,thread2,NULL);
    
    printf("Main joining...\n");
    pthread_join(m1,NULL);
    pthread_join(m2,NULL);

   // pthread_mutex_destroy(&mutex_vis);

    //printf("Cantidad de visitantes: %d ",visitantes);

    return 0;
}