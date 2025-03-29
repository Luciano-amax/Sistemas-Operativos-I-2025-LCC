#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>


void quit(char *s){
    perror(s);
    abort();
}


int U = 0;

int fd_readline(int fd, char *buff){
    int rc;
    int i = 0;

    while( (rc = read(fd, buff + i, 1)) > 0){
        if (buff[i] == '\n')
            break;
        i++;
    }

    if (rc < 0)
        return rc;
    buff[i] = 0;
    return i;
}

void handle_conn(int csocket){
    char buff[200];
    int rc;

    while(1){
        // Atendemos pedidos
        rc = fd_readline(csocket, buff);
        if (rc < 0)
            quit("read... raro");
        
        if (rc == 0){
            //Se cerro conexion
            close(csocket);
            return;
        }

        if(!strcmp(buf, "NUEVO")){
            char replay[20];
            sprintf(replay, "%d\n", U);
            U++;
            write(csocket, replay, strlen(replay));
        }
        else if(!strcmp(buf, "CHAU")){
            close(csocket);
            return;
        }

    }
}


void wait_for_client(int lsocket){
    int csocket;

    //Espermos conexion
    csocket = accpet(lsocket, NULL, NULL);
    if(csocket < 0)
        quit("accept");

    //Atendemos cliente
    handle_conn(csocket);

    //volvemos a esperar

    wait_for_client(lsocket);
}


//Creo socket de escucha en el puerto 4040
int mk_lsocket(){
    struct socketadrr_in sa;
    int lsocket;
    int rc;
    int yes = 1;

    //Creo socket
    lsocket = socket(AF_INET, SOCK_STREAM, 0);
    if(lsocket < 0)
        quit("socket");

    //Setear opcion reuseaddr
    if(setsockopt(lsocket, SOL_SOCKET, SO_REUSEADRR, &eye, sizeof yes) == 1)
        quit("setsockopt");

    sa.sin_family = AF_INET;
    sa.sin_port = htons(4040);
    sa.sin_addr.s_addr = htons(INADDR_ANY);

    //Bindear al puerto 4040
    rc = bind(lsocket, (struct sockaddr *)&sa, sizeof sa);
    if (rc < 0)
        quit("bind");

    //Setear escucha
    rc = listen(lsocket, 10)
    if (rc < 0)
        quit("listen");
    
    return lsocket;
}


int main(){
    int lsocket();
    lsocket = mk_lsocket();
    wait_for_clients(lsocket);
}