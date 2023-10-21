// This file define how client acts when enter port command
#include "../client_headers/client_funcs.h"
#include "../const.h"

int client_port(int client_sock, char* command){
    char ip[50];

    // printf("debug: %s\r\n", command);
    send(client_sock, command, strlen(command), 0);

    char server_response[256];
    int bytes_received = recv(client_sock, server_response, sizeof(server_response), 0);
    if (bytes_received <= 0) {
        // perror("Failed to receive response from server");
        printf("aa");
        return 1;
    }
    server_response[bytes_received] = '\0';

    if (strncmp(server_response, "200", 3) == 0) {
        printf("%s\r\n", server_response);
    } else {
        printf("Server Response Error!");
    }
}