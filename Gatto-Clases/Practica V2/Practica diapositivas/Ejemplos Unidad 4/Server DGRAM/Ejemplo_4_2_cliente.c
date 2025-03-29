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
    int socketCliente = socket(AF_LOCAL, SOCK_DGRAM, 0);

    struct sockaddr_un server, cliente;
    cliente.sun_family = AF_UNIX;
    strcpy(cliente.sun_path, SOCKET_CLIENTE_FILE);

    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCKET_SERVER_FILE);

    bind(socketCliente, (struct sockaddr *)&cliente, sizeof(cliente));

    char buff_enviar[500];
    strcpy(buff_enviar, "Esto es un mensaje del cliente");
    char buff_recibir[500];
    
    socklen_t sizeServer = sizeof(server);
    sendto(socketCliente, buff_enviar, 500, 0, (struct sockaddr *)&server, sizeServer);
    printf("CLiente envio: %s\n", buff_enviar);

    recvfrom(socketCliente, buff_recibir, 500, 0, (struct sockaddr *)&server, &sizeServer);
    printf("Cliente recibio: %s\n", buff_recibir);


    close(socketCliente);
    remove(SOCKET_CLIENTE_FILE);
    return 0;
}

