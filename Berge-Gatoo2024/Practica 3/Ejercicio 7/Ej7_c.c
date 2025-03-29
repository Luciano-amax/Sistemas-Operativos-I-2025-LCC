#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <semaphore.h>
sem_t tabaco, papel, fosforos, otra_vez;

int fumadores_dormidos = 0;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void awake() {
    pthread_cond_broadcast(&cond);
    sem_post(&otra_vez);
}

void dormir() {
    // Para entrar tenes que tener el mutex
    fumadores_dormidos++;
    if(fumadores_dormidos < 2)
        pthread_cond_wait(&cond,&mutex);
    else
    {
        pthread_cond_broadcast(&cond);
        fumadores_dormidos = 0;
    }
}

void agente(){
    while (1) {
        sem_wait(&otra_vez);
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
    while (1) {
        pthread_mutex_lock(&mutex);
        int t,p,f;
        sem_getvalue(&tabaco, &t);
        sem_getvalue(&papel, &p);
        sem_getvalue(&fosforos,&f);

        if(t & p)
        {
            sem_wait(&tabaco);
            sem_wait(&papel);
            awake();
            fumar(1);
        } else
        {
            dormir();
        }
        pthread_mutex_unlock(&mutex);
    }
}

void * fumador2(void *arg){
    while (1) {
        pthread_mutex_lock(&mutex);
        
        int t,p,f;
        sem_getvalue(&tabaco, &t);
        sem_getvalue(&papel, &p);
        sem_getvalue(&fosforos,&f);

        if(f & t)
        {
            sem_wait(&fosforos);
            sem_wait(&tabaco);
            awake();
            fumar(2);
        } else
        {
            dormir();
        }
        pthread_mutex_unlock(&mutex);
    }
}

void * fumador3(void *arg){
    while (1) {
        pthread_mutex_lock(&mutex);
        
        int t,p,f;
        sem_getvalue(&tabaco, &t);
        sem_getvalue(&papel, &p);
        sem_getvalue(&fosforos,&f);

        if(p & f)
        {
            sem_wait(&papel);
            sem_wait(&fosforos);
            awake();
            fumar(3);
        } else
        {
            dormir();
        }

        pthread_mutex_unlock(&mutex);
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