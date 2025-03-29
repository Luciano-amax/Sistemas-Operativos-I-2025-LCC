#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

int main(){
    printf("Esperando cliente...\n");
    int socketServer = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server, cliente;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    int sizeCliente = sizeof(cliente);

    char mensajeMandado[1024], mensajeRecibido[1024];

    bind(socketServer, (struct sockaddr *)&server, sizeof(server));
    listen(socketServer, 0);
    int socketCliente = accept(socketServer, (struct sockaddr *)&cliente, &sizeCliente);

    //-------------- Recibo mensaje del cliente ------------------
    recv(socketCliente, mensajeRecibido, 1024, 0);
    printf("El mensaje mandado por el cliente fue: %s", mensajeRecibido);

    //-------------- Mando mensaje al cliente --------------------
    printf("Mando mensaje al cliente\n\n");
    strcpy(mensajeMandado, "Mensaje desde el servidor\n");
    send(socketCliente, mensajeMandado, 1024, 0);

    return 0;
}