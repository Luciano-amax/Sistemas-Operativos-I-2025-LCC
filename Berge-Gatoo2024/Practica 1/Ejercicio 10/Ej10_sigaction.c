#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

//#define _XOPEN_SOURCE 700
#define _GNU_SOURCE

int x;

void signhup_handler(int signal){
    sleep(1);
    if(signal == SIGUSR1){
        if(x == 0){
            printf("Hijo recibe mensaje, mando al padre\n");
            kill(getppid(), SIGUSR1);
        }
        else{
            printf("Padre recibe mensaje, mando al hijo\n");
            kill(x, SIGUSR1);
        }
    }
    return;
}

int main(){
    struct sigaction sa;
    sa.sa_handler = signhup_handler;
    
    x = fork();
    if (x == 0){
        sigaction(SIGUSR1, &sa, NULL);
        for(;;);
    }
    else{
        printf("Mando señal al hijo \n");
        kill(x, SIGUSR1);
        sigaction(SIGUSR1, &sa, NULL);
        wait(0);
        for(;;);
    }

    return 0;
}