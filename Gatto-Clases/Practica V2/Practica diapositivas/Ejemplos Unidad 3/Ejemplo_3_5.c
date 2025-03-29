#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void handler(int signal){
    if(signal == SIGHUP)
        printf("Recibi señal del padre\n");
    
    exit(1);
}


int main(){
    pid_t pid = fork();
    
    if(pid == 0){
        signal(SIGHUP, handler);
        for(;;);
    }
    else{
        sleep(1);
        printf("Mando señal al hijo\n");
        kill(pid, SIGHUP);
        wait(0);
    }

    return 0;
}