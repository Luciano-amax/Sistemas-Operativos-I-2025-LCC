#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKERT_CLIENT_FILE "../socket_client_file"
#define SOCKERT_SERVER_FILE "../socket_file"


int main(){

    int socket_fd = socket(AF_UNIX, SOCKET_DGRAM, 0);
    // "Boca del socket", luego hay que bindiar

    struct sockaddr_un addr_client= {
        AF_UNIX, SOCKERT_CLIENT_FILE;
    };

    struct sockaddr_un server_addr = {
        AF_UNIX, SOCKERT_SERVER_FILE ;
    };

    socket_t server_size = sizeof(server_addr);
    
    char buffer[] = "Hola Mundo";

    bind(socket_fd, (struct sockaddr *)&addr_client, sizeof(addr_client));

    sendto(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)server_addr, &server_size);

    printf("Client, Envio:%s \n", buffer);

    char buffer2[1024];

    recvfrom(socket_fd, buffer2, sizeof(buffer2), 0, (struct sockaddr *)server_addr, &server_size)

    printf("Client, Recibo:%s \n", buffer)2;

    close(socket_fd);

    remove(SOCKERT_CLIENT_FILE);

    return 0;
}