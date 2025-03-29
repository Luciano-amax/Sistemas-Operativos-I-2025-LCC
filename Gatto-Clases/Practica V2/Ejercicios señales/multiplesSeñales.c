#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void handleSignals(int sig){
    if(sig == SIGQUIT){
        printf("\nEn este programa no aceptamos esa señal >:(\n");
    }
    else if(sig == SIGINT){
        printf("\nLa señal de terminacion fue capturada una vez\n");
        signal(SIGINT, SIG_DFL);
    }
}

int main(){
    signal(SIGQUIT, handleSignals);
    signal(SIGINT, handleSignals);
    printf("Espero la señal de stop...\n");
    while(1);
    return 0;
}
