#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

int x;

void signhup_handeler(int signal){
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
    x = fork();
    if (x == 0){
        signal(SIGUSR1, signhup_handeler);
        pause();
    }
    else{
        printf("Mando señal al hijo \n");
        kill(x, SIGUSR1);
        signal(SIGUSR1, signhup_handeler);
        wait(0);
        pause();
    }

    return 0;
}