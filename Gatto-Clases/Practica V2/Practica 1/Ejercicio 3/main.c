#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){

    while(1){
        if(argc <= 1)
            printf("y\n");
        else{
            for(int i = 1; i < argc; i++)
                printf("%s ", argv[i]);
            printf("\n");
        }
    }
}