#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
    char buff[1024];
    int fd = open("archivoPrueba.txt", O_RDONLY);
    
    read(fd, buff, 500);
    printf("%s\n",buff);

    close(fd);
    return 0;
}