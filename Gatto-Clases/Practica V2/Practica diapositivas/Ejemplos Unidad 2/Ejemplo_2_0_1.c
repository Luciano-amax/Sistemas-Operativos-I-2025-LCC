#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(){
    int pid = getpid();
    char* path = "/home/ramiro/Desktop/Practica V2/Practica diapositivas/Ejemplos Unidad 2/progRemplazar";
    printf("Hola soy el programa 1, mi pid es %d\n", pid);
    
    execl(path, path, NULL, NULL, NULL);
    
    return 0;
}
