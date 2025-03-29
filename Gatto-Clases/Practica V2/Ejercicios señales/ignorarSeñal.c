#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void handler(int signal){
    if(signal == SIGINT){
        printf("\nLa señal para terminar no es ignorada :)\n");
        exit(1);
    }
}

int main(){
    signal(SIGTSTP, SIG_IGN);
    signal(SIGINT, handler);
    printf("Espero, la señal de stop va a ser ignorada...\n");
    while(1);
    return 0;
}
