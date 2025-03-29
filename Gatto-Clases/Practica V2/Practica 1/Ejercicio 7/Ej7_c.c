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
    int i = 0, j = 0, cantArgumentos = 0;
    char* file = NULL;

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
            if(str[i+1] == '>' && str[i+2] == '\0'){
                file = &str[i+3];
                cantArgumentos = cantArgumentos - 2;
                i = i + 2;
            }
            else{
                argumentos[j] = &str[i + 1];
                j++;
                cantArgumentos--;
            }
        }
        i++;
    }
    argumentos[j] = NULL;

    if(file){
        int fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        dup2(fd, STDOUT_FILENO);
    }
    
    execvp(str, argumentos);
}

int main(){
    while(1){
        char comando[1024];
        printf("Usuario@Ubunut$: ");
        stringInput(comando);

        int pid = fork();
        if(pid == 0){
            ejecutar(comando);
            exit(0);
        }
        else {
            wait(0);
        }
    }

    return 0;
}
