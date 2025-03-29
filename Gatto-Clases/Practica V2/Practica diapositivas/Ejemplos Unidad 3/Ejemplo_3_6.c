#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


int main(){
    int sv[2];
    pipe(sv);
    
    pid_t pid = fork();

    char buff[1024];
    if(pid == 0){
        close(sv[1]);
        read(sv[0], buff, 500);
        printf("Hijo recibe: %s\n", buff);
    }
    else{
        strcpy(buff, "Esto es un mensaje por un pipe");
        close(sv[0]);
        printf("Padre escribe\n");
        write(sv[1], buff, 500);
    }

    return 0; 
}