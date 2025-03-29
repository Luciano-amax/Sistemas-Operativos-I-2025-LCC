#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(){
    pid_t pid = fork();

    if(pid == 0){
        printf("Primero habla el hijo, hola como va\n");
        exit(0);
    }
    else{
        wait(0);
        printf("Ahora habla el padre, todo bien boy\n");
    }

    return 0;
}