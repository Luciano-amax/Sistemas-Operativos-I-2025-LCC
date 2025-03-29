#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


int main(){

    int fd[2];
    char buffer[] = "hola mundo\n";
    char read_buffer;
    pipe(fd);

    pid_t pid = fork();

    if(pid == 0){

        printf("child corriendo...\n");

        close(fd[1]);
        int sizeread  =1;
        while(sizeread != 0){
            sizeread = read(fd[0],&read_buffer,1);
            printf("child leyo: %c\n",read_buffer);
        
        }
    }else{

        close(fd[0]);

        write(fd[1], buffer, strlen(buffer)+1);

        close(fd[1]);

        wait(NULL);
    }

    return 0;
}

// pipe[0] lectura
// pipe[1] escritura 
