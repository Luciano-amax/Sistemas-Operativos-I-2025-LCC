#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>



void seek_write(int fd, off_t posicion,const char c){
   
    lseek(fd,posicion,SEEK_SET); //pos es absoluta
    int res = write(fd,&c,1);

    return;

}

char seek_read(int fd,off_t posicion){
    char resC;
    lseek(fd,posicion,SEEK_SET); //pos es absoluta
    int res = read(fd,&resC,1);

    return resC;
}

void characterAtTime(char* str){
    

    setbuf(stdout, NULL);
    char* ptr;
    int i = 0;
    for(ptr = str; ptr[i]; i++){
        putc(ptr[i],stdout);
    }
    return;


}



int main(){

    int fd = open("./file",O_RDWR);
    __pid_t pid = fork();

    if(pid == 0){  //Child
    
        char c = seek_read(fd,13);
        printf("Child leyo %c\n",c);
        seek_write(fd,13,'a');
    
    }else{ //Parent
        char c = seek_read(fd,13);
        printf("Parent leyo %c\n",c);
        seek_write(fd,13,'b');
    }

    close(fd);
    return 0;
}

/*
seek_read() tiene dos funciones dentro, lseek() y read(). Los dos procesos llaman a la funcion

*/