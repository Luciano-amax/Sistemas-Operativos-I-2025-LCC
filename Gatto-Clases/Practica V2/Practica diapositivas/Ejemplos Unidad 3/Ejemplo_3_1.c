#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(int signal){
    if(signal == SIGFPE){
        printf("ERROR EN LA DIVISION\n");
        exit(-1);
    }
}


int deno = 0;
int main(){
    int r;
    signal(SIGFPE, handler);
    //raise(SIGFPE); 		//Eso genera la excepcion
    r = 1 / deno;
    return 0;
}
