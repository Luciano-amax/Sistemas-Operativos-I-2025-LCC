#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>


void stringInput(char* str){
    int i = 0;
    char ch;
    while((ch = getchar()) != '\n' && ch != EOF){
        str[i++] = ch; 
    }
    str[i] = '\0';

}

int main(){
    char comando[1024];
    char* argumentos[] = {NULL};

    while(1){
        int pid = fork();

        if(pid == 0){
            printf("\nUbunut@User: ");
            stringInput(comando);
            execvp(comando,argumentos);
        }
        else
            wait(0);
    }

    return 0;
}