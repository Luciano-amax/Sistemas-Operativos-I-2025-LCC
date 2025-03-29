#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t tabaco, papel, fosforos, otra_vez;
int fumadores_durmiendo = 0;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void despertar(){
    pthread_cond_broadcast(&cond);
    sem_post(&otra_vez);
}

void dormir(){
    fumadores_durmiendo++;
    if(fumadores_durmiendo < 2)
        pthread_cond_wait(&cond, &lock);
    else{
        pthread_cond_broadcast(&cond);
        fumadores_durmiendo = 0;
    }
}

void agente(){
    while (1) {
        sem_wait(&otra_vez);
        srand(time(NULL));
        int caso = random() % 3;
        if (caso != 0) sem_post(&fosforos);
        if (caso != 1) sem_post(&papel);
        if (caso != 2) sem_post(&tabaco);
    }
}

void fumar(int fumador){
    printf("Fumador %d: Puf! Puf! Puf!\n", fumador);
    sleep(1);
}

void * fumador1(void *arg){
    int t, p;
    while (1) {
        pthread_mutex_lock(&lock);
        sem_getvalue(&tabaco, &t);
        sem_getvalue(&papel, &p);

        if(p && t){
            sem_wait(&tabaco);
            sem_wait(&papel);
            fumar(1);
            despertar();
        }
        else
            dormir();
        pthread_mutex_unlock(&lock);
        
    }
}

void * fumador2(void *arg){
    int f, t;
    while (1) {
        pthread_mutex_lock(&lock);

        sem_getvalue(&fosforos, &f);
        sem_getvalue(&tabaco, &t);

        if(f && t){
            sem_wait(&fosforos);
            sem_wait(&tabaco);
            fumar(2);
            despertar();
        }
        else
            dormir();
        pthread_mutex_unlock(&lock);
        
    }
}

void * fumador3(void *arg){
    int p, f;
    while (1) {
        pthread_mutex_lock(&lock);

        sem_getvalue(&papel, &p);
        sem_getvalue(&fosforos, &f);

        if(p && f){
            sem_wait(&papel);
            sem_wait(&fosforos);
            fumar(3);
            despertar();
        }
        else
            dormir();
        pthread_mutex_unlock(&lock);
        
    }
}

int main(){
    pthread_t s1, s2, s3;
    sem_init(&tabaco, 0, 0);
    sem_init(&papel, 0, 0);
    sem_init(&fosforos, 0, 0);
    sem_init(&otra_vez, 0, 1);

    pthread_create(&s1, NULL, fumador1, NULL);
    pthread_create(&s2, NULL, fumador2, NULL);
    pthread_create(&s3, NULL, fumador3, NULL);

    agente();
    return 0;
}
