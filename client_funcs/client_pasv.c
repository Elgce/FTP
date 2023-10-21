// This file define how client acts when enter pasv command
#include "../client_headers/client_funcs.h"
#include "../const.h"

struct IPPort client_pasv(int client_sock, char* command){
    struct IPPort ip_port = {0};
    send(client_sock, command, strlen(command), 0);
    char server_response[256];
    int bytes_received = recv(client_sock, server_response, sizeof(server_response), 0);
    if (bytes_received <= 0){
        perror("Failed to receive response from server");
        return;
    }
    server_response[bytes_received] = '\0';
    if (strncmp(server_response, "227", 3) == 0){
        printf("%s\r\n", server_response);
        unsigned int h1, h2, h3, h4, p1, p2;
        sscanf(server_response, "227 Entering Passive Mode (%u,%u,%u,%u,%u,%u)", &h1, &h2, &h3, &h4, &p1, &p2);
        snprintf(ip_port.ip, sizeof(ip_port.ip), "%u.%u.%u.%u", h1, h2, h3, h4);
        ip_port.port = (int)(256 * p1 + p2);
    } else{
        printf("Server response error\r\n");
    }
    return ip_port;
}