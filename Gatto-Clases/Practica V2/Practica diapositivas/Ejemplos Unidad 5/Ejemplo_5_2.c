#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


void* thread(){
    printf("Hello World\n");
}

int main(){
    pthread_t id;
    pthread_create(&id, NULL, thread, NULL);
    pthread_join(id, NULL);

    return 0;
}