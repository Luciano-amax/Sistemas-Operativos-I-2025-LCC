#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// int main(){
//     char buff[1024];
//     char path[]= "./example.txt";
//     int fd = open(path,O_RDONLY);
//     int num_read = read(fd,buff,5);
//     //pid_t pid = fork();
//     printf("%d\n",num_read);
//    buff[num_read] = '\0';
//    printf("%s\n",buff);
//
//     return 0;
// 
// }

//padre -> hijo ->(padre2 hijo2)
//      -> padre ->(padre3 hijo3)
//pid -> Process ID


/*  #define BASUALDO 3
int main(){
    while(1){
        pid_t pid = fork();
        if(pid == 0){
            execl()
        }
    }

    return 0;
} */


int main(){
    int status;
    pid_t pid = fork();

    if(pid == 0){
        char buff[256];
        char salida[] = "exit";
        while(1){
            fgets(buff,20,stdin);//entrada es Hola_\0
            printf("buff: %s\n",buff);
            if(!strcmp(buff,salida)){
                printf("ENTRO");
                exit(0);
            }else{
                puts("cuak");
                system(buff);
            }
        }
    }else{
        wait(&status);
        exit(0);
    }

    return 0;
}