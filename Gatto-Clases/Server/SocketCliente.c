#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>


#define SOCKET_SERVER_FILE "../socket_server_file"
#define SOCKET_CLIENTE_FILE "../socket_cliente_file"

int main(){
    int socket_cliente = socket(AF_LOCAL, SOCK_DGRAM, 0);
    
    struct sockaddr_un server, cliente;
    cliente.sun_family = AF_UNIX;
    strcpy(cliente.sun_path, SOCKET_CLIENTE_FILE);

    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCKET_SERVER_FILE);

    socklen_t server_size = sizeof(server);

    char buffer[] = "Hola Mundo";
    char buffer2[1024];

    bind(socket_cliente, (struct sockaddr *)&cliente, sizeof(cliente));

    sendto(socket_cliente, buffer, sizeof(buffer), 0, (struct sockaddr *)&server, server_size);
    printf("El server envio: %s\n", buffer);

    recvfrom(socket_cliente, buffer2, sizeof(buffer2), 0, (struct sockaddr *)&server, &server_size);
    printf("El server recibio: %s\n", buffer2);


    close(socket_cliente);
    remove(SOCKET_CLIENTE_FILE);

    return 0;
}