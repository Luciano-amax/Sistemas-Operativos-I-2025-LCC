#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/un.h>

#define SERVER_FILE "../SEREVER_FILE"
#define CLIENTE_FILE "../CLIENTE_FILE"

int main(){
    int socketCliente = socket(AF_LOCAL, SOCK_DGRAM, 0);

    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SERVER_FILE);

    int sizeServer = sizeof(server);
    
    struct sockaddr_un cliente;
    cliente.sun_family = AF_UNIX;
    strcpy(cliente.sun_path, CLIENTE_FILE);


    char mensajeRecibdo[1024], mensajeMandado[1024];

    //----------- Hago bindeo --------------------------------
    bind(socketCliente, (struct sockaddr*)&cliente, sizeof(cliente));    

    //----------- Mando mensaje al cliente -------------------
    printf("Cliente manda mensaje al servidor\n");
    strcpy(mensajeMandado, "Mensaje desde el cliente\n");
    sendto(socketCliente, mensajeMandado, 1024, 0, (struct sockaddr *)&server, sizeof(server));

    //----------- Recibo mensaje del cliente ------------------
    recvfrom(socketCliente, mensajeRecibdo, 1024, 0, (struct sockaddr *)&server, &sizeServer);
    printf("El cliente recibio el siguiente mensaje: %s\n", mensajeRecibdo);


    remove(CLIENTE_FILE);
    return 0;
}