#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>




    void handler_child(int sig){
        printf("Me mandaron la señal \n");
        exit(0);
    }

int main(){



    int valor = 3;

    pid_t pid = fork();

    if(pid == 0){

        struct sigaction sa;
        sa.sa_flags = SA_RESTART;
        sa.sa_handler = handler_child;


        sigaction(SIGINT,&sa,NULL);
        printf("child corriendo...\n");
        for(;;){

        }
    }else{
        sleep(1);
        kill(pid,SIGINT);
        wait(NULL);
        printf("Quien llega %d\n",pid);
    }

    return 0;
}

// pipe[0] lectura
// pipe[1] escritura 
