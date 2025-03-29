#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

int ret = 19;
void *thread(void *arg) {

  pthread_exit(&ret);
}

int main() {
  pthread_t thid;
  int *ret;

  pthread_create(&thid, NULL, thread, NULL) != 0;

  pthread_join(thid, (void**)&ret);

  printf("thread exited with '%d'\n", *((int*)ret));

  return 0;
}