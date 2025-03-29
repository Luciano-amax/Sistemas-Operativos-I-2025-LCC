#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(){
    printf("PID del programa original: %d\n", getpid());
    int pid = fork();
    if(pid == 0)
        printf("Soy el hijo y mi PID es: %d\n", getpid());
    else
        printf("Soy el padre y mi PID es: %d\n", getpid());

    return 0;
}