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
    int cantArgumentos = 0;
    
    for(int i = 0; str[i] != '\0'; i++)
        if (str[i] == ' ')
            cantArgumentos++;

    char** argumentos = malloc(sizeof(char*) * (cantArgumentos + 1));
    int j = 0;
    for(char* token = strtok(str, " "); token != NULL; token = strtok(NULL, " "))
        argumentos[j++] = token;
    argumentos[j] = NULL;


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