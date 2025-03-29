#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <fcntl.h>

void stringInput(char* str){
    int i = 0;
    char ch;
    while((ch = getchar()) != '\n' && ch != EOF){
        str[i++] = ch; 
    }
    str[i] = '\0';
}

void ejecutar(char* str){
    int cantArgumentos = 0;
    int cambio = 1;
    char* newFile;

    for(int i = 0; str[i] != '\0'; i++)
        if (str[i] == ' ')
            cantArgumentos++;

    char** argumentos = malloc(sizeof(char*) * (cantArgumentos + 1));
    int j = 0;
    for(char* token = strtok(str, " "); token != NULL; token = strtok(NULL, " ")){

        if (!strcmp(token, ">")){
            newFile = strtok(NULL, " ");
            cambio = 0;
        }
        else if (cambio != 0)
            argumentos[j++] = token;
    }
    argumentos[j] = NULL;

    if (cambio == 0){
        int fd = open(newFile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        dup2(fd, STDOUT_FILENO);
    }
    
    execvp(str, argumentos);
}

int main(){
    char comando[1024];

    while(1){
        int pid = fork();

        if(pid == 0){
            printf("\nUbunut@User: ");
            stringInput(comando);
            ejecutar(comando);
        }
        else
            wait(0);
    }

    return 0;
}