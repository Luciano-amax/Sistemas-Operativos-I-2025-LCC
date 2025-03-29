#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/un.h>

#define SERVER_FILE "../SEREVER_FILE"

int main(){
    int socketServer = socket(AF_LOCAL, SOCK_DGRAM, 0);
    printf("Server esperando...\n");

    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SERVER_FILE);

    struct sockaddr_un cliente;
    int sizeCliente = sizeof(cliente);

    char mensajeRecibdo[1024], mensajeMandado[1024];

    //----------- Hago bindeo --------------------------------
    bind(socketServer, (struct sockaddr*)&server, sizeof(server));    

    //----------- Recibo mensaje del cliente ------------------
    recvfrom(socketServer, mensajeRecibdo, 1024, 0, (struct sockaddr *)&cliente, &sizeCliente);
    printf("El server recibio el siguiente mensaje: %s", mensajeRecibdo);

    //----------- Mando mensaje al cliente -------------------
    printf("Ahora el server manda respuesta\n\n");
    strcpy(mensajeMandado, "Mensaje desde el servidor\n");
    sendto(socketServer, mensajeMandado, 1024, 0, (struct sockaddr *)&cliente, sizeof(cliente));

    remove(SERVER_FILE);
    return 0;
}