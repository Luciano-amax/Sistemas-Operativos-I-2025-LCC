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
#include <semaphore.h>

#define BUFFER_SIZE 10

sem_t sem_lugares, sem_ocupados;

int buffer[BUFFER_SIZE];
int num = 0;

void* productor(){
    int add = 0;
    for(int i = 0; i < 20000; ++i){
        printf("Waiting...");
    
        sem_wait(&sem_lugares);
        buffer[add] = 1;
        add = (add + 1) % BUFFER_SIZE;
        num++;
        sem_post(&sem_ocupados);

        printf("Produce: %d\n", i);
    }
}

void* consumidor(){
    int rem = 0;
    int i = 0;

    while(1){
        sem_wait(&sem_ocupados);
        printf("Waiting...\n");
        i = buffer[rem];
        rem = (rem + 1) % BUFFER_SIZE;
        num--;

        sem_post(&sem_lugares);
        printf("Consume: %d\n", i);
    }
}


int main (){
    sem_init(&sem_lugares, 0, BUFFER_SIZE);
    sem_init(&sem_ocupados, 0, 0);     // 0 por usar pthread

    

    return 0;
}























