#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


void* thread_1(void* arg){
    pthread_t id = *((pthread_t *) arg);
    printf("Primer thread: Hello\n");
    pthread_join(id, NULL);
}

void* thread_2(void* arg){
    printf("Segundo thread: Wolrd\n");
}

int main(){
    pthread_t id1,id2;
    pthread_create(&id1, NULL, thread_1, (void *)&id2);
    pthread_create(&id1, NULL, thread_2, NULL);
    
    pthread_join(id1, NULL);

    return 0;
}
