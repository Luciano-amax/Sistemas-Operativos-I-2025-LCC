#include <stdio.h>

int main(int argc, char** argv){

    while(1){
        if(argc == 1)
            printf("y\n");
        else
            printf("%s\n", argv[1]);
    }
    return 0;
}