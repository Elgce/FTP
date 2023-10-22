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
    char ret_message[BUFFER_SIZE];
    while(1){
        printf("> ");
        fgets(command, sizeof(command), stdin);
        char* tmp_command = strtok(command, "\n\r");
        char clean_command[BUFFER_SIZE];
        snprintf(clean_command, sizeof(clean_command), "%s\r\n", tmp_command);
        if (strncmp(clean_command, "PORT", 4) == 0){
            ipport = client_port(client_sock, clean_command);
            if (file_sock.is_active == 1){
                close(file_sock.sock_fd);
                file_sock.is_active = 0;
            }
        }
        else if (strncmp(clean_command, "PASV", 4) == 0){
            ipport = client_pasv(client_sock, clean_command);
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
        else if (strncmp(clean_command, "RETR", 4) == 0 || strncmp(clean_command, "STOR", 4) == 0 || strncmp(clean_command, "LIST", 4) == 0){
            send(client_sock, clean_command, strlen(clean_command), 0);
            if (file_sock.is_active == 0){
                file_sock.sock_fd = socket(AF_INET, SOCK_STREAM, 0);
                if (file_sock.sock_fd == -1){
                    perror("Could not create socket");
                    exit(EXIT_FAILURE);
                }
                file_sock.addr.sin_family = AF_INET;
                file_sock.addr.sin_addr.s_addr = INADDR_ANY;
                file_sock.addr.sin_port = htons(ipport.port);
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
            
            char server_response[BUFFER_SIZE];
            int recv_size = recv(client_sock, server_response, sizeof(server_response), 0);
            server_response[recv_size] = '\0';
            printf("%s\r\n", server_response);
            
            if (strncmp(clean_command, "RETR", 4) == 0){
                client_retr(file_sock.sock_fd, clean_command);
                file_sock.is_active = 0;
                close(file_sock.sock_fd);
            }
            else if (strncmp(clean_command, "STOR", 4) == 0){
                client_stor(file_sock.sock_fd, clean_command);
                file_sock.is_active = 0;
                close(file_sock.sock_fd);
            }
            else if (strncmp(clean_command, "LIST", 4) == 0){
                recv_size = recv(file_sock.sock_fd, server_response, sizeof(server_response), 0);
                server_response[recv_size] = '\0';
                printf("%s\r\n", server_response);
            }
            recv_size = recv(client_sock, server_response, sizeof(server_response), 0);
            server_response[recv_size] = '\0';
            printf("%s\r\n", server_response);
        }
        else if (strcmp(clean_command, "SYST\r\n") == 0 || strcmp(clean_command, "TYPE I\r\n") == 0){
            send(client_sock, clean_command, strlen(clean_command), 0);
            bzero(ret_message, BUFFER_SIZE);
            recv(client_sock, ret_message, BUFFER_SIZE, 0);
            printf("%s\r\n", ret_message);
        }
        else if (strcmp(clean_command, "QUIT\r\n") == 0 || strcmp(clean_command, "ABOR\r\n") == 0){
            send(client_sock, clean_command, strlen(clean_command), 0);
            bzero(ret_message, BUFFER_SIZE);
            recv(client_sock, ret_message, BUFFER_SIZE, 0);
            printf("%s\r\n", ret_message);
            if (file_sock.is_active == 1){ // close this client and release resouses
                close(file_sock.sock_fd);
            }
            close(client_sock);
            return;
        }
        else if (strncmp(clean_command, "MKD", 3) == 0 || strncmp(clean_command, "CWD", 3) == 0 || strncmp(clean_command, "PWD", 3) == 0 \
        || strncmp(clean_command, "RMD", 3) == 0 || strncmp(clean_command, "RNFR", 4) == 0 || strncmp(clean_command, "RNTO", 4) == 0){
            send(client_sock, clean_command, strlen(clean_command), 0);
            bzero(ret_message, BUFFER_SIZE);
            recv(client_sock, ret_message, BUFFER_SIZE, 0);
            printf("%s\r\n", ret_message);
        }
        else{
            printf("Error: Wrong command input or not implemented.\r\n");
        }
    }
    close(client_sock);
    return;
}

int main(int argc, char **argv) {
    int port = 21; // default port

    for (int i = 1; i < argc - 1; i++) { 
        if (strcmp(argv[i], "-port") == 0) {
            port = atoi(argv[i + 1]);
            break;
        }
    }
    char ipaddr[BUFFER_SIZE] = "127.0.0.1";  

    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-ip") == 0) {
            strncpy(ipaddr, argv[i + 1], sizeof(ipaddr) - 1);
            ipaddr[sizeof(ipaddr) - 1] = '\0';  // 确保字符串结束符存在
            break;
        }
    }


    int client_sock;
    struct sockaddr_in address;

    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(client_sock == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if(inet_pton(AF_INET, ipaddr, &address.sin_addr) <= 0) {
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