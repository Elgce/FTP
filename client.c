#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "const.h"
#include "client_headers/client_login.h"

void client_handler(int client_sock){
    int login_flag = client_login(client_sock);
    if (login_flag == 1){
        printf("login failed!\r\n");
        close(client_sock);
        return; // show login failed.
    }
    while(1){
        // TODO
    }
    close(client_sock);
    return;
}

int main(int argc, char **argv) {
    int client_sock;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];

    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(client_sock == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(SERVER_PORT);

    if(inet_pton(AF_INET, SERVER_IP, &address.sin_addr) <= 0) {
        perror("Invalid address or Address not supported");
        exit(EXIT_FAILURE);
    }

    if(connect(client_sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Connection error");
        exit(EXIT_FAILURE);
    }

    client_handler(client_sock);

    // while(1) {
    //     printf("Enter command: ");
    //     fgets(buffer, sizeof(buffer), stdin);
    //     send(client_sock, buffer, strlen(buffer), 0);
    //     int read_size = recv(client_sock, buffer, sizeof(buffer), 0);
    //     if(read_size > 0) {
    //         printf("Response: %s\n", buffer);
    //     }
    // }

    return 0;
}
