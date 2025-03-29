#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(){
    int socket[2];          //0: socket hijo       1: socket padre
    socketpair(AF_LOCAL, SOCK_DGRAM, 0, socket);

    pid_t pid = fork();
    char buff[1024];

    if(pid == 0){
        close(socket[1]);
        sleep(1);
        //----------- Hijo recibe mensaje -------------------
        read(socket[0], buff, 1024);
        printf("El mensaje del padre era: %s", buff);

        //----------- Hijo manda mensaje -------------------

        printf("Hijo manda mensaje\n");
        strcpy(buff, "Mesaneje mandado por el hijo\n");
        write(socket[0], buff, 1024);

        exit(0);
    }
    else{
        close(socket[0]);
        //---------------- Padre manda mensaje ---------------

        printf("Padre manda mensaje\n");
        strcpy(buff, "Mesaneje mandado por el padre\n");
        write(socket[1], buff, 1024);
        sleep(1);

        //---------------- Padre recibe mensaje ---------------
        wait(0);

        read(socket[1], buff, 1024);
        printf("El mensaje del hijo era: %s", buff);
    }

}