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
            if(str[i+1] == '>'){
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

    if(file != NULL){
        int fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        dup2(fd, STDOUT_FILENO);
    }
    
    execvp(str, argumentos);
}

int main(){
    while(1){   
        int salidaOriginal = dup(STDOUT_FILENO);
        int entradaOriginal = dup(STDIN_FILENO);

        int cantComandos = 1;
        char comando[1024];
        printf("Usuario@Ubunut$: ");
        stringInput(comando);
        int largoComando = strlen(comando);

        for(int i = 0; comando[i] != '\0'; i++){
            if(comando[i] == '|'){
                cantComandos++;
                comando[i] = '\0';
            }
        }

        int i = 0;
        int in = dup(entradaOriginal), out = dup(salidaOriginal);

        while(cantComandos != 0){
            dup2(in, STDIN_FILENO);

            if(cantComandos == 1){
                out = dup(salidaOriginal);
                cantComandos--;
            }
            else{
                int pip[2];
                pipe(pip);
                in = pip[0];
                out = pip[1];
                cantComandos--;
            }

            //Dependiendo si el comadndo es el primero o el ultimo el out cambia
            dup2(out, STDOUT_FILENO);
            close(out);
            
            int pid = fork();
            if(pid == 0){
                ejecutar(&comando[i]);
            }
            else {
                wait(0);
            }

            while(comando[i] != '\0')
                i++;
            i++;
            
            dup2(entradaOriginal, STDIN_FILENO);
            dup2(salidaOriginal, STDOUT_FILENO);
        }

        
    }

    return 0;
}