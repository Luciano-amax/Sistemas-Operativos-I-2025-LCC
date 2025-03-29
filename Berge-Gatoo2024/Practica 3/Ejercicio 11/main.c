#include <stdio.h>
#include <pthread.h>
#include "channel.h"

struct channel c;

void* lector() {
    while(1)
    {
        printf("Leo: %d\n", chan_read(&c));
        sleep(3);
    }
}

void* escritor() {
    for(int val = 0; ; val++)
    {
        chan_write(&c,val);
    }
}

int main() {
    pthread_t id1, id2;

    channel_init(&c);

    pthread_create(&id1, NULL, lector, NULL);
    pthread_create(&id2, NULL, escritor, NULL);

    for(;;);
    return 0;
}