#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>

char lseek_read(int fd, off_t pos){
    char buffer;
    lseek(fd, pos, SEEK_SET);

    read(fd, &buffer, 1);
    return buffer;
}

void lseek_write(int fd, off_t pos, const char c){
    lseek(fd, pos, SEEK_SET);
    // Se mete el child y hace read()
    write(fd, &c, 1);
}

int main(){

    setbuf(stdout, NULL);

    int fd = open("./archivo.txt", O_RDWR);
    pid_t pid = fork();
    
    if(pid == 0){
       char c = lseek_read(fd, 13);
       printf("Child leyo: %c\n", c);

       lseek_write(fd, 13, 'z');
       printf("Child escribio: z\n");
    }
    else{
       char c = lseek_read(fd, 13);
       printf("Parent leyo: %c\n", c);

       lseek_write(fd, 13, 'x');
       printf("Parent escribio: x\n");
    }
    
    return 0;
}

//No se puede asegurar el orden de ejecucion
//Pueden ocurrir los lseek a la vez, lo que hace que se ejecuten 2 read seguidos

/*
Se hacen los lseek a la vez, primero el del hijo, luego el del padre, el del hijo mueve el cabezal
Ambos hacen solo la lectura, lueho el hijoo hace el read (mueve el cabezal) y final el padre quiere
escribir una mas adelante

Sucede esto 
13 24 15 26

otmica => No hay resultados parciales, empieza y termina, nada se mete en medio

*/
