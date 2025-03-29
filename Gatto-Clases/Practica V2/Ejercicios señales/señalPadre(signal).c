#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void handler(int sig){
    if(sig == SIGHUP)
        printf("La señal recibida fue SIGHUP\n");
        exit(0);
}

int main(){

    pid_t pid = fork();
    if(pid == 0){
        printf("En el Hijo:\n");
        signal(SIGHUP, handler);
        for(;;);
    }
    else{
        printf("Padre le manda señal al hijo\n");
        usleep(1);
        kill(pid, SIGHUP);
        wait(0);
    }
}