#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>


int denom = 0;
void handler(int s) {
    printf("ouch!\n");
    exit(0);
    denom = 1;
}

int main() {
    int r;
    signal(SIGFPE, handler);
    r = 1 / denom;
    printf("r = %d\n", r);
    return 0;
}

/*
Lo que pasa al correr el programa es que al querer hacer la division por cero
se prdocue una excepcion (se ejecuta el handler), pero se queda "prendida" la señal
que genero la division, por eso al querer retomar el programa vuelve a caer en la 
excepccion una y otra vez.

*/