#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "const.h"
#include "client_headers/client_funcs.h"

void client_handler(int client_sock){
    int login_flag = client_login(client_sock);
    if (login_flag == 1){
        printf("login failed!\r\n");
        close(client_sock);
        return; // show login failed.
    }
    char command[BUFFER_SIZE];
    while(1){
        printf("> ");
        fgets(command, sizeof(command), stdin);
        char* clean_command = strtok(command, "\n\r");
        if (strncmp(clean_command, "PORT", 4) == 0){
            client_port(client_sock, command);
        }
        else if (strncmp(clean_command, "RESV", 4) == 0){
            printf("wrong!\r\n");
        }
        else{
            printf("Error: Wrong command input or not implemented.");
        }
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

    return 0;
}
