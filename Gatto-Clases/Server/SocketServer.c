#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>


#define SOCKET_SERVER_FILE "../socket_server_file"

int main(){
    printf("Server esta esperando...\n");
    int socket_server = socket(AF_LOCAL, SOCK_DGRAM, 0);
    
    struct sockaddr_un server, cliente;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCKET_SERVER_FILE);

    socklen_t cliente_size = sizeof(cliente);

    char buffer[1024];
    char buffer2[] = "Que mi**** queres";

    bind(socket_server, (struct sockaddr *)&server, sizeof(server));


    recvfrom(socket_server, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliente, &cliente_size);
    printf("El server recibio: %s\n", buffer);

    sleep(3);

    sendto(socket_server, buffer2, sizeof(buffer2), 0, (struct sockaddr *)&cliente, cliente_size);
    printf("El server envio: %s\n", buffer2);

    close(socket_server);
    remove(SOCKET_SERVER_FILE);

    return 0;
}