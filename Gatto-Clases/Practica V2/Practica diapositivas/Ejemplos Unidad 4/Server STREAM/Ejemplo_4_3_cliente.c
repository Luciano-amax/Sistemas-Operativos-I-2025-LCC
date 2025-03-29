#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_SERVER_FILE "../socket_server_file"
#define SOCKET_CLIENTE_FILE "../socket_cliente_file"

int main(){
    int socketCliente = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un server, cliente;
    cliente.sun_family = AF_UNIX;
    strcpy(cliente.sun_path, SOCKET_CLIENTE_FILE);

    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCKET_SERVER_FILE);
    socklen_t sizeServer = sizeof(server);

    bind(socketCliente, (struct sockaddr *)&cliente, sizeof(cliente));

    char buff_enviar[500];
    strcpy(buff_enviar, "Esto es un mensaje del cliente");
    char buff_recibir[500];

    connect(socketCliente, (struct sockaddr *)&server, sizeServer);

    send(socketCliente, buff_enviar, 500, 0);
    printf("CLiente envio: %s\n", buff_enviar);

    recv(socketCliente, buff_recibir, 500, 0);
    printf("Cliente recibio: %s\n", buff_recibir);


    close(socketCliente);
    remove(SOCKET_CLIENTE_FILE);
    return 0;
}

