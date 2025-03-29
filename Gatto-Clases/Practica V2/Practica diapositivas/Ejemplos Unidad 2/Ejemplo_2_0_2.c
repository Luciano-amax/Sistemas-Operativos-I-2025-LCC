#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(){
    int pid = getpid();
    char* path = "/Desktop/Practica V2/Practica diapositivas/Ejemplos Unidad 2/Ejemplo_2_0_2.c";
    printf("Hola soy el programa 2, mi pid es %d\n", pid);
    return 0;
}
