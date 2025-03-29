#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define N_FILOSOFOS 5
#define ESPERA 5000000

// Vamos a tomar al primer filosofo como el zurdo

pthread_mutex_t tenedor[N_FILOSOFOS];
pthread_mutex_t * izq(int i) { return &tenedor[i]; }
pthread_mutex_t * der(int i) { return &tenedor[(i+1) % N_FILOSOFOS]; }

void pensar(int i){
    printf("Filosofo %d pensando...\n", i);
    usleep(random() % ESPERA);
}

void comer(int i){
    printf("Filosofo %d comiendo...\n", i);
    usleep(random() % ESPERA);
}

void tomar_tenedores_diestros(int i){
    pthread_mutex_lock(der(i));
    pthread_mutex_lock(izq(i));
}

void dejar_tenedores_diestros(int i){
    pthread_mutex_unlock(der(i));
    pthread_mutex_unlock(izq(i));
}

void tomar_tenedores_zurdos(int i){
    pthread_mutex_lock(izq(i));
    pthread_mutex_lock(der(i));
}

void dejar_tenedores_zurdos(int i){
    pthread_mutex_unlock(izq(i));
    pthread_mutex_unlock(der(i));
}

void * filosofo(void *arg){
    int i = arg - (void*)0;
    while (1) {
        if(i == 0){
            tomar_tenedores_zurdos(i);
            comer(i);
            dejar_tenedores_zurdos(i);
        }
        else{
            tomar_tenedores_diestros(i);
            comer(i);
            dejar_tenedores_diestros(i);
        }
        
        pensar(i);
    }
}

int main(){
    pthread_t filo[N_FILOSOFOS];
    int i;
    for (i = 0; i < N_FILOSOFOS; i++)
        pthread_mutex_init(&tenedor[i], NULL);

    for (i = 0; i < N_FILOSOFOS; i++)
        pthread_create(&filo[i], NULL, filosofo, i + (void*)0);
    
    pthread_join(filo[0], NULL);
    return 0;
}