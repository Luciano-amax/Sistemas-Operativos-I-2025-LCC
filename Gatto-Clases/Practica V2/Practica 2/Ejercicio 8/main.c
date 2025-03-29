#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>

#define N 5

int visitantes = 0;
void * proc(void *arg) {
    int i;
    int id = arg - (void*)0;
    for (i = 0; i < N; i++) {
        int c;
        sleep(1);
        c = visitantes;             // Se puede usar if para mejorar
        sleep(1);
        visitantes = c + 1;         // Se puede usar if para mejorar
        /* sleep? */
    }

    return NULL;
}

int main(){
    pthread_t id1, id2;
    pthread_create(&id1, NULL, proc, NULL);
    pthread_create(&id2, NULL, proc, NULL);

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);

    printf("Visitantes Totales: %d\n", visitantes);

    return 0;
}
