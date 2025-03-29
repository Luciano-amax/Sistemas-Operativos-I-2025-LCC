#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define N_SILLAS 10

pthread_mutex_t mutex_buffer =  PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c_cons = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_prod = PTHREAD_COND_INITIALIZER;

int buffer[N_SILLAS];
int num = 0;

void me_pagan(){
    pthread_mutex_unlock(&mutex_buffer);
}

void pagando(){
    pthread_mutex_unlock(&mutex_buffer);
}

//Clientes
void* clientes(){
    int add = 0;

    for(int i = 0; i < 20000; ++i){
        pthread_mutex_lock(&mutex_buffer);

        while(num >= N_SILLAS){
            //cliente se fue
            printf("Esta lleno y me fui\n");
            pthread_cond_wait(&c_prod, &mutex_buffer);
        }

        //Llego el cliente
        printf("Llego cliente %d\n",i);
        buffer[add] = i;
        add = (add + 1) % N_SILLAS;
        num++;

        //Avsio al barbero que estoy listo
        pthread_cond_signal(&c_cons);
        pagando();
    }
}

//Barbero
void* barbero(){
    int rem = 0;
    int i = 0;

    while(1){

        pthread_mutex_lock(&mutex_buffer);

        while(num == 0){
            //Me voy a dormir, espero clietne
            printf("ZzZ...\n");
            pthread_cond_wait(&c_cons, &mutex_buffer);
        }
    
        //CLiente recibe corte
        printf("Le corto a cliente\n");
        i = buffer[rem];
        rem = (rem + 1) % N_SILLAS;
        num--;

        //Llamo proximo cliente
        pthread_cond_signal(&c_prod);
        me_pagan();
    }
}


int main (){
    pthread_t tid1, tid2;

    //Agrega por si falla
    pthread_create(&tid1, NULL, barbero, NULL);
    pthread_create(&tid2, NULL, clientes, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return 0;
}






















