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

#define MAX_THREADS 4
#define MAX_EVENTS 4

struct node{
    char* key;
    char* value;
    struct node *next;
};

struct node *createNode(){
    struct node *temp = malloc(sizeof(struct node));
    temp->key = NULL;
    temp->value = NULL;
    temp->next = NULL;
    return temp;
}

struct node *addNode(struct node* inicio, char *value, char *key){
    struct node *temp, *p;
    temp = createNode();
    
    
    temp->key = malloc(sizeof(char) * (strlen(key) + 1));
    temp->value = malloc(sizeof(char) * (strlen(value) + 1));
    
    strcpy(temp->key, key);
    strcpy(temp->value, value);


    if(inicio == NULL)
        inicio = temp;
    else{
        p  = inicio;
        
        while(p->next != NULL)
            p = p->next;
            
        p->next = temp;
        
    }
    return inicio;
}

struct node *delateNode(struct node *inicio, char* key){
    struct node *p  = inicio, *temp = NULL;
    if(inicio == NULL)
        return inicio;
    if(!strcmp(inicio->key, key)){
        temp = inicio->next;

        free(inicio->key);
        free(inicio->value);
        free(inicio);

        return temp;
    }
    else{

        while(p != NULL && strcmp(p->next->key, key) != 0)
            p = p->next;

        if(p != NULL){
            temp = p->next->next;

            free(p->next->key);
            free(p->next->value);
            free(p->next);

            p->next = temp;
        }
        
        return inicio;
    }
}

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

char* findValue(struct node* inicio, char* key){
    struct node *p = inicio;
    char* buf;
    if(inicio == NULL){
        buf = malloc(sizeof(char) * (1 + strlen("NOTFOUND")));
        strcpy(buf, "NOTFOUND");
    }
    else if(inicio->next == NULL){
        if(!strcmp(p->key, key)){
            buf = malloc(sizeof(char) * (1+ strlen(p->value)));
            strcpy(buf, p->value);
        }
        else{
            buf = malloc(sizeof(char) * (1 + strlen("NOTFOUND")));
            strcpy(buf, "NOTFOUND");
        }
    }
    else{
        while(p != NULL && strcmp(p->key, key) != 0)
            p = p->next;
        if(p == NULL){
            buf = malloc(sizeof(char) * (1 + strlen("NOTFOUND")));
            strcpy(buf, "NOTFOUND");
        }
        else{
            buf = malloc(sizeof(char) * (1+ strlen(p->value)));
            strcpy(buf, p->value);
        }
    }
    return buf;
}

void handle_conn(int csock){
    struct node *inicio = NULL;

        char *k, *v;
		char *buf = readline(csock, &v, &k);
        
        if(!strcmp(buf, "GET")){
            char* reply = findValue(inicio, k);
            write(csock, reply, strlen(reply));
            write(csock, "\n", strlen("\n"));
        }
        else if(!strcmp(buf, "PUT")){
            inicio = addNode(inicio, v, k);
            write(csock, "OK\n", 3);
        }
        else if(!strcmp(buf, "DEL")){
            inicio = delateNode(inicio, k);
            write(csock, "OK\n", 3);
        }
        else if(!strcmp(buf, "EXIT\n")){
            close(csock);
            return;
        }
        else{
            write(csock, "EINVAL\n", 7);
        }
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
                       ev.events = EPOLLIN | EPOLLET;
                       ev.data.fd = conn_sock;
                       if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
                                   &ev) == -1) {
                           perror("epoll_ctl: conn_sock");
                           exit(EXIT_FAILURE);
                       }
                   } else {
                        handle_conn(events[n].data.fd);
                       //do_use_fd(events[n].data.fd);
                   }
               }
           }
}

/* Crea un socket de escucha en puerto 3942 TCP */
int mk_lsock()
{
	struct sockaddr_in sa;
	int lsock;
	int rc;

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

int main()
{
	int lsock;
	lsock = mk_lsock();
    int listen_sock, conn_sock, nfds, epollfd;

    /* Code to set up listening socket, 'listen_sock',
        (socket(), bind(), listen()) omitted */

    if (epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[MAX_THREADS];
    for(int i=0;i<MAX_THREADS;i++)
    {
        pthread_create(threads+i, NULL, wait_for_clients, &lsock);
        pthread_join(threads[i],NULL);
    }
}