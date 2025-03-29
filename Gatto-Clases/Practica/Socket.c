#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKERT_SERVER_FILE "../socket_file"

int main(){

    int socket_fd = socket(AF_UNIX, SOCKET_DGRAM, 0);
    // "Boca del socket", luego hay que bindiar

    struct sockaddr_un addr_server = {
        AF_UNIX, SOCKERT_SERVER_FILE;
    };

    struct socketaddr_un cli_addr;
    socket_t cli_size = sizeof(cli_addr);
    
    char buffer[1024]

    bind(socket_fd, (struct sockaddr *)&addr_server, sizeof(addr_server));

    recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cli_addr, &cli_size)

    printf("SERVER, Recibo:%s \n", buffer);

    sendto(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cli_addr, &cli_size);

    printf("SERVER, Envio:%s \n", buffer);

    close(socket_fd);

    remove(SOCKERT_SERVER_FILE);

    return 0;
}