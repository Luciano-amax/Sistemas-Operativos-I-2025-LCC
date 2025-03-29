#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

int pid;
void handler(int signal){
    sleep(1);
    if(signal == SIGUSR1){
        if(pid != 0){
            printf("Padre: Ping\n");
            kill(pid, SIGUSR1);
        }
        else{
            printf("Hijo: Pong\n");
            kill(getppid(), SIGUSR1);
        }
    }
}       

int main(){
    signal(SIGUSR1, handler);

    pid = fork();
    if(pid != 0){
        printf("Padre: Ping\n");
        sleep(1);
        kill(pid, SIGUSR1);
        for(;;);
    }
    else{
        for(;;);
    }
}
