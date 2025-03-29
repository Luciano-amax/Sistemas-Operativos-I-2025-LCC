#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/mman.h>

int main(){
    const char;
    int shm_fd;
    int *shared_memory;

    int not_shared_memory = 6;
    shm_fd = shm_open(main,O_CREAT | O_RDWR, 0666);

    ftruncate(shm_fd, PAGESIZE);


    return 0;
}