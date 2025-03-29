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

#include <pthread.h>
#define _GNU_SOURCE

int i = 2;

void *dummy(void *arg){
    printf("El argumetno es %i\n", *(int*)arg);
    //printf("%d\n", i);
    pthread_exit(&i);
}

int main(){
    setbuf(stdout, NULL);       //Liempio buffer
    pthread_t id;
    int* ptr;
    int argument = 5;
    pthread_create(&id, NULL, dummy, &argument);

    //pthread_yield();           //libera CPU, permito que el therad corra

    pthread_join(id, (void**)&ptr);      //Espero a que el thread termine
    printf("Valor de salida del thread %ld: %i\n", id, *ptr);
    
    printf("main\n");
    return 0;
}

//El thread muere si o si, si no lo matas lo mata el CPU
//Todo lo declarado en el thread se libera al terminar





