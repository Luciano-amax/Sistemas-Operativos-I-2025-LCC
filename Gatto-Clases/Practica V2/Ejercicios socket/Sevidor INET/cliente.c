#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

int main(){
    int socketCliente = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    int sizeServer = sizeof(server);

    struct sockaddr_in cliente;
    cliente.sin_family = AF_INET;
    cliente.sin_port = htons(PORT);
    cliente.sin_addr.s_addr = INADDR_ANY;

    char mensajeMandado[1024], mensajeRecibido[1024];

    bind(socketCliente, (struct sockaddr *)&cliente, sizeof(cliente));
    connect(socketCliente, (struct sockaddr *)&server, sizeServer);

    //-------------- Mando mensaje al server --------------------
    printf("Cliente manda mensaje al servidor\n\n");
    strcpy(mensajeMandado, "Mensaje desde el cliente\n");
    send(socketCliente, mensajeMandado, 1024, 0);

    //-------------- Recibo mensaje del server ------------------
    recv(socketCliente, mensajeRecibido, 1024, 0);
    printf("El mensaje mandado por el servidor fue: %s", mensajeRecibido);

    return 0;
}