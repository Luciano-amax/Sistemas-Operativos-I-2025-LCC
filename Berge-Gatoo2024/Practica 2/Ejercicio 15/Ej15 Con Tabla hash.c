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

#include <sys/time.h>
#include <sys/select.h>
#include <sys/epoll.h>

#include <string.h>
#include <pthread.h>

#include <signal.h>

#define MAX_THREADS 4
#define MAX_EVENTS 4

#include "hash.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

HashTable hashtable;

char* readline(int fd,  char **v, char** k){
    int espacio = 0;
    char *buf = malloc(sizeof(char) * 200);
	read(fd, buf, 200);

    for(int i = 0; buf[i] != '\0'; i++){
        if(buf[i] == ' '){
            if(espacio == 0){
                *v = &buf[i + 1];
                espacio++;
            }
            if(espacio == 1)
                *k = &buf[i + 1];
            buf[i] = '\0';
        }
    }

    return buf;
}

void handle_conn(int csock){
        char *k, *v;
		char *buf = readline(csock, &v, &k);

        if(strlen(buf) == 0)
        {
            /* linea vacia, se cerró la conexión */
            free(buf);
            close(csock);
            return;
        }

        if(!strcmp(buf, "GET")){
            pthread_mutex_lock(&mutex);
            char* reply = hashtable_find_value(hashtable, k);
            pthread_mutex_unlock(&mutex);
            if(reply == NULL)
                write(csock, "NOT FOUND\n", 10);
            else
            {
               write(csock, reply, strlen(reply));
               write(csock, "\n", 1);
               free(reply);
            }
        }
        else if(!strcmp(buf, "PUT")){
            pthread_mutex_lock(&mutex);
            hashtable = hashtable_add(hashtable, v, k);
            pthread_mutex_unlock(&mutex);
            write(csock, "OK\n", 3);
        }
        else if(!strcmp(buf, "DEL")){
            pthread_mutex_lock(&mutex);
            hashtable = hashtable_delete(hashtable, k);
            pthread_mutex_unlock(&mutex);
            write(csock, "OK\n", 3);
        }
        else if(!strcmp(buf, "EXIT\n")){
            close(csock);
        }
        else{
            write(csock, "EINVAL\n", 7);
        }

        free(buf);
}

void* wait_for_clients(void* ptr_lsock){
    int listen_sock = *((int*)ptr_lsock);

    struct epoll_event ev, events[MAX_EVENTS];
    int conn_sock, nfds, epollfd;

    epollfd = epoll_create1(0);
           if (epollfd == -1) {
               perror("epoll_create1");
               exit(EXIT_FAILURE);
           }


    ev.events = EPOLLIN;
           ev.data.fd = listen_sock;
           if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
               perror("epoll_ctl: listen_sock");
               exit(EXIT_FAILURE);
           }


    for (;;) {
               nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
               if (nfds == -1) {
                   perror("epoll_wait");
                   exit(EXIT_FAILURE);
               }

               for (int n = 0; n < nfds; ++n) {
                   if (events[n].data.fd == listen_sock) {
                       conn_sock = accept(listen_sock, NULL, NULL);
                       if (conn_sock == -1) {
                           perror("accept");
                           exit(EXIT_FAILURE);
                       }
                       ev.events = EPOLLIN | EPOLLOUT;
                       ev.data.fd = conn_sock;
                       if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
                                   &ev) == -1) {
                           perror("epoll_ctl: conn_sock");
                           exit(EXIT_FAILURE);
                       }
                   } else {
                        handle_conn(events[n].data.fd);
                   }
               }
           }
}

/* Crea un socket de escucha en puerto 3942 TCP */
int mk_lsock()
{
	struct sockaddr_in sa;
	int lsock;

	/* Crear socket */
	lsock = socket(AF_INET, SOCK_STREAM, 0);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(3942);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Bindear al puerto, en todas las direcciones disponibles */
	bind(lsock, (struct sockaddr *)&sa, sizeof sa);

	/* Setear en modo escucha */
	listen(lsock, 10);

	return lsock;
}

void sigint_handler(int signal){
    if(signal == SIGINT){
        printf("\nClosing server...\n");
        hashtable = hashtable_destroy(hashtable);
        exit(EXIT_SUCCESS);
    }
}


int main()
{
	int lsock;
	lsock = mk_lsock();

    hashtable = hashtable_init(1000);

    printf("Creating threads...\n");
    pthread_t threads[MAX_THREADS];
    for(int i=0;i<MAX_THREADS;i++)
    {
        printf("Thread %d is listening...\n", i+1);
        pthread_create(threads+i, NULL, wait_for_clients, &lsock);
    }

    signal(SIGINT, sigint_handler);

    for(;;);
}