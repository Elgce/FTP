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
    struct TCPConnection file_sock = {0};
    struct IPPort ipport = {0};
    char command[BUFFER_SIZE];
    while(1){
        printf("> ");
        fgets(command, sizeof(command), stdin);
        char* clean_command = strtok(command, "\n\r");
        if (strncmp(clean_command, "PORT", 4) == 0){
            ipport = client_port(client_sock, command);
            if (file_sock.is_active == 1){
                close(file_sock.sock_fd);
                file_sock.is_active = 0;
            }
        }
        else if (strncmp(clean_command, "PASV", 4) == 0){
            ipport = client_pasv(client_sock, command);
            if (file_sock.is_active == 1){
                // close old connection
                close(file_sock.sock_fd);
                file_sock.is_active = 0;
            }
            // open new connection
            file_sock.sock_fd = socket(AF_INET, SOCK_STREAM, 0);
            if(file_sock.sock_fd == -1){
                perror("Could not create socket");
                exit(EXIT_FAILURE);
            }
            file_sock.addr.sin_family = AF_INET;
            file_sock.addr.sin_addr.s_addr = INADDR_ANY;
            file_sock.addr.sin_port = htons(ipport.port);
            if (inet_pton(AF_INET, ipport.ip, &file_sock.addr.sin_addr) <= 0){
                perror("Invalid address or Address not supported");
                exit(EXIT_FAILURE);
            }
            if (connect(file_sock.sock_fd, (struct sockaddr *)&file_sock.addr, sizeof(file_sock.addr)) < 0){
                perror("Connection error");
                exit(EXIT_FAILURE);
            }
            else{
                printf("New connection established\r\n");
            }
            file_sock.is_active = 1;
        }
        else if (strncmp(clean_command, "RETR", 4) == 0 || strncmp(clean_command, "STOR", 4) == 0){
            if (file_sock.is_active == 0){
                file_sock.sock_fd = socket(AF_INET, SOCK_STREAM, 0);
                if (file_sock.sock_fd == -1){
                    perror("Could not create socket");
                    exit(EXIT_FAILURE);
                }
                file_sock.addr.sin_family = AF_INET;
                file_sock.addr.sin_addr.s_addr = INADDR_ANY;
                file_sock.addr.sin_port = htonl(ipport.port);
                if (bind(file_sock.sock_fd, (struct sockaddr *) & file_sock.addr, sizeof(file_sock.addr)) < 0){
                    perror("Bind failed");
                    exit(EXIT_FAILURE);
                }
                listen(file_sock.sock_fd, 5);
                int c = sizeof(struct sockaddr_in);
                file_sock.sock_fd = accept(file_sock.sock_fd, (struct sockaddr *) & file_sock.addr, (socklen_t*)&c);
                if (file_sock.sock_fd < 0){
                    perror("Accept failed");
                    exit(EXIT_FAILURE);
                }
                else{
                    printf("New connection established\r\n");
                }
                file_sock.is_active = 1;
            }
        }
        else{
            printf("Error: Wrong command input or not implemented.\r\n");
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
    address.sin_addr.s_addr = INADDR_ANY;
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
