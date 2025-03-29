#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>

#define SERVER_FILE "SEREVER_FILE"

int main(){
    int socketServer = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SERVER_FILE);

    struct sockaddr_un cliente;
    int sizeCliente = sizeof(cliente);

    bind(socketServer, (struct sockaddr *)&server, sizeof(server));

    printf("Esperando cliente...\n");
    listen(socketServer, 0);
    int socketCliente = accept(socketServer, (struct sockaddr *)&cliente, &sizeCliente);

    char mensajeRecibido[1024], mensajeMandado[1024];

    //------------------- Recibo mensaje del cliente -----------------------
    recv(socketCliente, mensajeRecibido, 1024, 0);
    printf("El mensaje recibido por el servidor fue: %s", mensajeRecibido);

    //------------------- Mando mensaje al cliente -----------------------
    printf("Ahora el server manda respuesta\n\n");
    strcpy(mensajeMandado, "Esto es un mensaje del servidor\n");
    send(socketCliente, mensajeMandado, 1024, 0);

    remove(SERVER_FILE);
    return 0;
}