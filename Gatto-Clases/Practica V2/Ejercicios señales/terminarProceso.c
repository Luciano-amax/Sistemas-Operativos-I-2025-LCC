#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void handler(int signal){
    if(signal == SIGTSTP){
        printf("\nEl programa recibio la señal de stop y termino\n");
        exit(1);
    }
    else if(signal == SIGINT){
        printf("\nLa señal no es correcta\n");
    }
}

int main(){
    signal(SIGTSTP, handler);
    signal(SIGINT, handler);
    printf("Espero la señal de stop...\n");
    while(1);
    return 0;
}