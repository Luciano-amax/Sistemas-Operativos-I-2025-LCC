#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[]){
    int len = strlen("/home/ramiro/Desktop/Practica V2/Practica diapositivas/");
    char* path = malloc(sizeof(char) * (len + strlen(argv[1]) + 1));
    strcpy(path, "/home/ramiro/Desktop/Practica V2/Practica diapositivas/");
    strcat(path, argv[1]);
    
    
    while(1){
        pid_t pid = fork();
        if (pid == 0){
            execl(path, path, NULL, NULL, NULL);
        }
        else{
            sleep(atoi(argv[2]));
        }
    }
    return 0;
}
