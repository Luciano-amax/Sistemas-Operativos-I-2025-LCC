
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define N_SILLAS 5

// Tiempos del programa
#define TIEMPO_ENTRADA_CLIENTE 1 // Los clientes entran cada (TIEMPO_ENTRADA_CLIENTE) segundos
#define TIEMPO_PREPARACION_CORTE 1 // Tiempo que le toma al cliente prepararse para que el barbero lo corte
#define TIEMPO_CORTE_BARBERO 3 // Tiempo que le toma al barbero realizar un corte
#define TIEMPO_PAGO_CLIENTE 1 // Nota, este el tiempo que le toma al cliente pagarle al barbero y tambien es el tiempo que le toma al barbero recibir el pago

int sillas_ocupadas = 0;

pthread_mutex_t sillas_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t barbero_cliente_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t c_no_vacio = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_barbero = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_cliente = PTHREAD_COND_INITIALIZER;

void me_cortan();
void cortando();
void me_pagan();
void pagando();
void me_voy();

int esta_vacio() {
    return sillas_ocupadas == 0;
}

int esta_lleno() {
    return sillas_ocupadas == N_SILLAS;
}

void barberia() {

    while(1) {
        pthread_mutex_lock(&sillas_lock);
        if(esta_lleno())
            printf("Nuevo cliente: Barberia llena, me voy\n");
        else
        {
            printf("Nuevo cliente: Barberia no llena, entro\n");
            sillas_ocupadas++;
            if(sillas_ocupadas == 1)
            {
                pthread_cond_signal(&c_no_vacio);
                printf("Nuevo cliente: Barbero dormido, despierto al barbero\n");
            }
        }
        pthread_mutex_unlock(&sillas_lock);
        sleep(TIEMPO_ENTRADA_CLIENTE);
    }

}

void* cliente() {

    while(1) {
        pthread_mutex_lock(&sillas_lock);
        while(esta_vacio())
        {
            printf("Barberia vacia, Zzz...\n");
            pthread_cond_wait(&c_no_vacio, &sillas_lock);
        }
        pthread_mutex_unlock(&sillas_lock);

        me_cortan();
        pagando();
        me_voy();
    }
} 

void* barbero() {
    pthread_mutex_lock(&barbero_cliente_lock);
    pthread_cond_wait(&c_barbero, &barbero_cliente_lock);
    while(1) {
        // Tomo el lock y espero a que el siguiente cliente este listo
        cortando();
        me_pagan();
    }
}


////////////////////////////////////////

void me_cortan() {
    printf("Cliente: Me cortan\n");

    sleep(TIEMPO_PREPARACION_CORTE);
    // Le digo al barbero que me corte y me duermo

    pthread_mutex_lock(&barbero_cliente_lock);
    pthread_cond_signal(&c_barbero);
    pthread_cond_wait(&c_cliente, &barbero_cliente_lock);
}

void cortando() {
    printf("Barbero: Cortando\n");
    // Le corto el pelo a mi cliente
    sleep(TIEMPO_CORTE_BARBERO);

    printf("Barbero: Corte terminado\n");
    // Termine, le aviso a mi cliente que me tiene que pagar
    pthread_cond_signal(&c_cliente);
    pthread_cond_wait(&c_barbero, &barbero_cliente_lock);

}

void pagando() {
    printf("Cliente: Pagando\n");
    // Le pago al barbero
    sleep(TIEMPO_PAGO_CLIENTE);

    // Le aviso al barbero que le pague y me duermo
    pthread_cond_signal(&c_barbero);
    pthread_cond_wait(&c_cliente, &barbero_cliente_lock);
}

void me_pagan() {
    // Mi cliente me paga

    printf("Barbero: Me pagan\n");
    sleep(TIEMPO_PAGO_CLIENTE);

    // Le digo a mi cliente que se vaya, yo quedo a la espera
    pthread_cond_signal(&c_cliente);
    pthread_cond_wait(&c_barbero, &barbero_cliente_lock);
}

void me_voy() {
    printf("Cliente: Ya me cortaron, me voy\n");
    // Me voy de la peluqueria y dejo el asiento libre
    pthread_mutex_lock(&sillas_lock);
    sillas_ocupadas--;
    pthread_mutex_unlock(&sillas_lock);

    pthread_mutex_unlock(&barbero_cliente_lock);
}


int main (){
    pthread_t id1,id2;
    pthread_create(&id1, NULL, barbero, NULL);
    pthread_create(&id2, NULL, cliente, NULL);

    barberia();
    return 0;
}
