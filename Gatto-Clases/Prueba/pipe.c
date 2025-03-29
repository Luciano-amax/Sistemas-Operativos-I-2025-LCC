#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(){
    int pipefd[2];
    pipe(pipefd);
    int pid = fork();

    char str[100];
    if (pid == 0){
        close(pipefd[1]);                   // 1: escritura,  0: lectura
        read(pipefd[0], str, 100);
        printf("Soy el hijo, %s\n", str);
    }
    else{
        printf("Soy el padre, mando mensaje a mi hijo\n");
        sleep(1);
        strcpy(str, "Hola que tal maquina\n");
        close(pipefd[0]);
        write(pipefd[1], str, 100);
    }


    return 0;
}