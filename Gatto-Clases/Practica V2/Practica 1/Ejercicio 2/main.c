#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	int fd = open("Hola.txt", 0);
	int fd_copy = dup(fd);
	
	close(fd_copy);
	char buff[100];
	read(fd, buff, 100);
	printf("%s\n", buff);
	return 0;
}
