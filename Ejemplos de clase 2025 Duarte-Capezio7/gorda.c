#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


void handler_Stop(int singum){
    printf("Me stoppearon\n");

    exit(0);
}

void handler2(int signum){
    printf("Se presinoo CTRL+C\n");
    signal(SIGINT, SIG_DFL);
}

int main(){
    void (*signalReturn)(int);
    printf("hola\n");
    
    
    printf("buenas\n");
    signalReturn = signal(SIGTSTP, handler2);
    

    raise(SIGTSTP);
    return 0;
}