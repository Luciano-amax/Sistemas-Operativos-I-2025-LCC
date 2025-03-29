#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(int signal){
    if(signal == SIGTSTP){
        printf("Señal recibida\n");
        exit(0);
    }
}


int main(){
    signal(SIGTSTP, handler);

    while(1){
        printf("Esperando...\n");
    }

    printf("Termino\n");
    return 0; 
}
