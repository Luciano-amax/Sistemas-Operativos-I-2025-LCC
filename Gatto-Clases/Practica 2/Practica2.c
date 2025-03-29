#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>

#include <pthread.h>
#include <assert.h>
#define _GNU_SOURCE

int personasDentro = 0;
int turn;
int flag[2];

void* molinete1(void* cant){
    do{
        flag[0] = 1;
        turn = 1;

        while(flag[1] && turn == 1);
    
        personasDentro = personasDentro + *((int *)cant);
        printf("Personas en el jardin: %d (m1)\n", personasDentro);
        sleep(1);

        flag[0] = 0;

    }while(1);
}

void* molinete2(void* cant){
    do{
        flag[1] = 1;
        turn = 0;

        while(flag[0] && turn == 0);
    
        personasDentro = personasDentro + *((int *)cant);
        printf("Personas en el jardin: %d (m2)\n", personasDentro);
        sleep(1);


        flag[1] = 0;

    }while(1);
}

int main(){
    pthread_t m1, m2;
    int cant = 1;

    pthread_create(&m1, NULL, molinete1, &cant);
    pthread_create(&m2, NULL, molinete2, &cant);

    while(1);

    return 0;
}