#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *inputString(FILE* fp, size_t size){
    // El tamanio se extiende a medida que la entrada crece
    char *str;
    int ch;
    size_t len = 0;
    str = realloc(NULL, sizeof(*str)*size); //size es el tamanio inicial
    if(!str)return str;
    while(EOF!=(ch=fgetc(fp)) && ch != '\n'){
        str[len++]=ch;
        if(len==size){
            str = realloc(str, sizeof(*str)*(size+=16));
            if(!str)return str;
        }
    }
    str[len++]='\0';

    return realloc(str, sizeof(*str)*len);
}

int main() {
    while(1) {
        char* str = inputString(stdin, 100);
        int pid = fork();
        if(pid == 0)
        {
            char* arglist[] = {NULL};
            // Soy el hijo!
            execvp(str, arglist);
        }
        wait(NULL);
    }
    return 0;
}