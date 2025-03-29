#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <sys/types.h>
void stringInput(char* str){
    int i = 0;
    char ch;

    while((ch = getchar()) != '\n' && ch != EOF){
        str[i++] = ch;
    }
    str[i] = '\0';
}

void ejecutar(char* str){
    int i = 0, j = 0, cantArgumentos = 0;

    while(str[i] != '\0'){
        if(str[i] == ' '){
            cantArgumentos++;
            str[i] = '\0';
        }
        i++;
    }

    char** argumentos = (char**)malloc(sizeof(char*) * (cantArgumentos + 1));

    i = 0;
    argumentos[j++] = str;
    while(cantArgumentos != 0){
        if(str[i] == '\0'){
            argumentos[j] = &str[i + 1];
            j++;
            cantArgumentos--;
        }
        i++;
    }
    
    argumentos[j] = NULL;
    execvp(str, argumentos);
}

int main(){
    while(1){
        char comando[1024];
        
        int pid = fork();
        if(pid == 0){
            printf("Usuario@Ubunut$: ");
            stringInput(comando);
            ejecutar(comando);
            exit(0);
        }
        else {
            wait(0);
        }
    }

    return 0;
}
