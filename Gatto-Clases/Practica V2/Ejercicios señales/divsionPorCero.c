#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int x = 0;

void handler(int signal){
    if(signal == SIGFPE)
        printf("No se puede dividir por cero\n");
    exit(1);
}

int main(){
    int r;
    signal(SIGFPE, handler);
    r = 1 / x;
    return 0;
}