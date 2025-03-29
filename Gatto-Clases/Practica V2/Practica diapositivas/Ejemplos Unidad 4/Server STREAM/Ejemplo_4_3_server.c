#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_SERVER_FILE "../socket_server_file"

int main(){
    printf("Server a la espera...\n");
    int socketServer = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un server, cliente;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCKET_SERVER_FILE);

    bind(socketServer, (struct sockaddr *)&server, sizeof(server));

    char buff_enviar[500];
    strcpy(buff_enviar, "Esto es un mensaje del servidor");
    char buff_recibir[500];

    socklen_t sizeCliente = sizeof(cliente);
    listen(socketServer, 0);
    int socketCliente = accept(socketServer, (struct sockaddr *)&cliente, &sizeCliente);

    recv(socketCliente, buff_recibir, 500, 0);
    printf("Server recibio: %s\n", buff_recibir);

    send(socketCliente, buff_enviar, 500, 0);
    printf("Server mando: %s\n", buff_enviar);

    close(socketServer);
    remove(SOCKET_SERVER_FILE);
    return 0;
}
