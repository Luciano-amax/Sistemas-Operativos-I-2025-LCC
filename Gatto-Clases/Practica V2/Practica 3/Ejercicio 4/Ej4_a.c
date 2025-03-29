#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define M 5
#define N 5
#define SZ 8

sem_t lock, lugar, ocupado;

/*
 * El buffer guarda punteros a enteros, los
 * productores los consiguen con malloc() y los
 * consumidores los liberan con free()
 */
int *buffer[SZ];
int add = 0, rem = 0;

void enviar(int *p)
{
	buffer[add] = p;
    	add = (add + 1) % SZ;
	return;
}

int * recibir()
{
	int *p = buffer[rem];
    	rem = (rem + 1) % SZ;
	return p;
}

void * prod_f(void *arg){
	int id = arg - (void*)0;
	while (1) {
        sem_wait(&lugar);
        sem_wait(&lock);
		sleep(random() % 3);

		int *p = malloc(sizeof *p);
		*p = random() % 100;
		printf("Productor %d: produje %p->%d\n", id, p, *p);
		enviar(p);
        sem_post(&lock);

        sem_post(&ocupado);
	}
	return NULL;
}

void * cons_f(void *arg){
	int id = arg - (void*)0;
	while (1) {
        sem_wait(&ocupado);
        sem_wait(&lock);
		sleep(random() % 3);

		int *p = recibir();
		printf("Consumidor %d: obtuve %p->%d\n", id, p, *p);
		free(p);
        sem_post(&lock);

        sem_post(&lugar);
	}
	return NULL;
}

int main(){
    sem_init(&lock, 0, 1);
    sem_init(&lugar, 0, SZ);
    sem_init(&ocupado, 0, 0);

	pthread_t productores[M], consumidores[N];
	int i;

	for (i = 0; i < M; i++)
		pthread_create(&productores[i], NULL, prod_f, i + (void*)0);

	for (i = 0; i < N; i++)
		pthread_create(&consumidores[i], NULL, cons_f, i + (void*)0);

	pthread_join(productores[0], NULL); /* Espera para siempre */
	return 0;
}
