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
        printf("EL Hijo registra la señal SIGHUP\n");
        
        struct sigaction sa;
        sa.sa_flags = SA_RESTART;
        sa.sa_handler = &handler;

        sigaction(SIGHUP, &sa, NULL);

        for(;;);
    }
    else{
        printf("Padre le manda señal al hijo\n");
        sleep(1);
        kill(pid, SIGHUP);
        wait(0);
    }
}