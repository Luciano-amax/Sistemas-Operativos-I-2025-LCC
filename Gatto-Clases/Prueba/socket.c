#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>


int main(){
    int sv[2];              //0: hijo,   1: padre
    int socket = socketpair(AF_LOCAL, SOCK_STREAM, 0, sv);
    char str[100];
    
    int pid = fork();

    if (pid != 0){
        close(sv[0]);
        printf("Soy el padre, mando mensaje a mi hijo\n");
        sleep(1);
        strcpy(str, "Hola que tal maquina\n");
        write(socket, str, 100);

    }
    else{
        close(sv[1]);
        read(socket, str, 100);
        printf("Soy el hijo, %s\n", str);

    }


    return 0;
}