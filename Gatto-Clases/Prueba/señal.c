#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

void signhup_handeler(int signal){
    sleep(1);
    if(signal == SIGHUP){
        printf("Señal recibida \n");
    }
    exit(1);
}

int main(){
    int x = fork();
    if (x == 0){
        signal(SIGHUP, signhup_handeler);
        for(;;);
    }
    else{
        printf("Mando señal al hijo \n");
        kill(x, SIGHUP);
        wait(0);
    }

    return 0;
}