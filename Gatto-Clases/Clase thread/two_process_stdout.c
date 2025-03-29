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

void charatatime(char *str){
    char *ptr;
    int c;
    for(ptr = str; (c = *ptr++) != 0;){
        putc(c, stdout);     //Escribe en un fd un char
    }
}

int main(){
    pid_t pid = fork();
    
    if(pid == 0){
        charatatime("Child\n");
    }
    else{
        charatatime("Parent\n");
    }
    
    return 0;
}





