#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
    int pid = fork();

    pid_t pid1 = fork();
    pid_t pid2 = fork();
    
    printf("pid 1: %d, pid 2: %d \n", pid1, pid2);

    return 0;
}
/*
pid 1: 13219, pid 2: 13221 
pid 1: 13219, pid 2: 0 
pid 1: 13220, pid 2: 13222 
pid 1: 13220, pid 2: 0 
pid 1: 0, pid 2: 13223 
pid 1: 0, pid 2: 13224 
pid 1: 0, pid 2: 0 
pid 1: 0, pid 2: 0 

*/