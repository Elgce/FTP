// This file define functions that used for login
#include "../server_headers/server_funcs.h"
#include "../const.h"

int server_login(int client_sock) {
    char buffer[BUFFER_SIZE];
    char server_name[] = "Anonymous";
    char response[BUFFER_SIZE];

    snprintf(response, sizeof(response), "220 %s server ready.\r\n", server_name);
    send(client_sock, response, strlen(response), 0);
    
    recv(client_sock, buffer, sizeof(buffer), 0);
    
    // judge if client is anonymous
    // in this work, we only allow anonymous login
    if (strcmp(buffer, "USER anonymous\r\n") == 0) {
        send(client_sock, "331 Guest login ok, send your complete e-mail address as the password.\r\n", strlen("331 Guest login ok, send your complete e-mail address as the password.\r\n"), 0);
        bzero(buffer, BUFFER_SIZE);
        recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (strncmp(buffer, "PASS ", 5) == 0) {
            send(client_sock, "230 Login successful.\r\n", strlen("230 Login successful.\r\n"), 0);
        }
        else{
            send(client_sock, "530 Wrong pwd input.\r\n", strlen("530 Wrong pwd input.\r\n"), 0);
            return 1;
        }
    } else {
        send(client_sock, "530 Invalid user.\r\n", strlen("530 Invalid user.\r\n"), 0);
        return 1;
    }
    return 0;
}