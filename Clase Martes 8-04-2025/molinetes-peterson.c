#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include <pthread.h>
//flag = 1 LOCK
//flag = 0 UNLOCK

int visitantes = 0;
//int flag = 0; // 0 -> RC esta desocupada, 1 -> RC esta ocupada
int turn = 0;//0 = no se asigno turno |1 = turno de thread1 | 2 = turno de thread2
#define NUM_VISITANTES 40000

int flag[2] = {0,0}; // 0 .> no tengo intencion de entrar a la RC | 1 -> tengo la intencion de netrar a la RC

void* thread1(void* v){
    
    for(int i = 0; i < NUM_VISITANTES/2;++i){
        
        //lock
        flag[0] = 1; //molinete1 intencion de entrar a RC
        turn = 2;
        while(flag[1] == 1 && turn == 2){;}

        //region critica
        visitantes++;
        printf("m1: %d\n",visitantes);
        
        //unlock
        flag[0] = 0;
    }
}
void* thread2(void* v){
    
    for(int i = 0; i < NUM_VISITANTES/2;++i){
        //lock
        flag[1] = 1; //molinete1 intencion de entrar a RC
        turn = 1;
        while(flag[0] == 1 && turn == 1){;}

        //region critica
        visitantes++;
        printf("m2: %d\n",visitantes);
        
        //unlock
        flag[1] = 0;
    }
}

int main(){
    pthread_t m1, m2;

    pthread_create(&m1, NULL ,thread1,NULL);
    pthread_create(&m2, NULL ,thread2,NULL);
    
    printf("Main joining...\n");
    pthread_join(m1,NULL);
    pthread_join(m2,NULL);

    printf("Cantidad de visitantes: %d ",visitantes);

    return 0;
}