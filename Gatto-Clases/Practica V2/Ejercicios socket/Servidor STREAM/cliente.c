#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>

#define SERVER_FILE "SEREVER_FILE"
#define CLIENTE_FILE "CLIENTE_FILE"

int main(){
    int socketCliente = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un cliente;
    cliente.sun_family = AF_UNIX;
    strcpy(cliente.sun_path, CLIENTE_FILE);

    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SERVER_FILE);
    int sizeServer = sizeof(server);

    bind(socketCliente, (struct sockaddr *)&cliente, sizeof(cliente));

    connect(socketCliente, (struct sockaddr *)&server, sizeServer);

    char mensajeRecibido[1024], mensajeMandado[1024];

    //------------------- Mando mensaje al server -----------------------
    printf("Mando mensaje al servidor\n\n");
    strcpy(mensajeMandado, "Esto es un mensaje del cliente\n");
    send(socketCliente, mensajeMandado, 1024, 0);

    //------------------- Recibo mensaje del servidor -----------------------
    recv(socketCliente, mensajeRecibido, 1024, 0);
    printf("El mensaje recibido por el cliente fue: %s", mensajeRecibido);


    remove(CLIENTE_FILE);
    return 0;
}