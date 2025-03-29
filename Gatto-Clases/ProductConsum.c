/*
Buffer variables de condicion

Se quedan bloqueados en el cond (hasta un signal)
                        -----> se quedan esperando el lock (No el signal)
(Se quedan bloqueados en cond)

-----------------------
Semaforos

Depositar
Lugares.down
cons_push
Ocupados.up
(Hacemos up)


Tomar
Ocupados.down
t = pop
Lugares.up
retornar t
(Hacemos down)

-----------------------------------

#include <semaphone.h>
        ---> para usar semaforos
Los usamos con thread

(Se deben destruir al terminar)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 10

pthread_mutex_t mutex_buffer =  PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c_cons = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_prod = PTHREAD_COND_INITIALIZER;

int buffer[BUFFER_SIZE];
int num = 0;

void* productor(){
    int add = 0;

    for(int i = 0; i < 20000; ++i){
        pthread_mutex_lock(&mutex_buffer);

        while(num > BUFFER_SIZE)
            pthread_cond_wait(&c_prod, &mutex_buffer);
        buffer[add] = i;
        add = (add + 1) % BUFFER_SIZE;
        num++;

        pthread_cond_signal(&c_cons);
        pthread_mutex_unlock(&mutex_buffer);

        printf("Produce: %d\n", i);
    }
}

void* consumidor(){
    int rem = 0;
    int i = 0;

    while(1){

        pthread_mutex_lock(&mutex_buffer);

        while(num == 0)
            pthread_cond_wait(&c_cons, &mutex_buffer);

        i = buffer[rem];
        rem = (rem + 1) % BUFFER_SIZE;
        num--;

        pthread_cond_signal(&c_prod);
        pthread_mutex_unlock(&mutex_buffer);
        printf("Consume: %d\n", i);
    }
}


int main (){
    pthread_t tid1, tid2;

    //Agrega por si falla
    pthread_create(&tid1, NULL, productor, NULL);
    pthread_create(&tid2, NULL, consumidor, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);


    return 0;
}























