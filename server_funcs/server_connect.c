// This file define how server acts when pasv & port
#include <string.h>
#include "../server_headers/server_funcs.h"
#include "../const.h"
static int pasv_port = SERVER_START;

struct IPPort server_pasv(int client_sock){
    struct IPPort ip_port = {0};
    pasv_port = (pasv_port + 1) % 65536;
    if (pasv_port < 20000){
        pasv_port = 20000;
    }
    ip_port.port = pasv_port;
    strcpy(ip_port.ip, SERVER_IP);
    char message[256];
    snprintf(message, sizeof(message), "227 Entering Passive Mode (127,0,0,1,%d,%d)", pasv_port / 256, pasv_port % 256);
    send(client_sock, message, strlen(message), 0);
    return ip_port;
}

struct IPPort server_port(int client_sock, char* buffer) {
    unsigned int h1, h2, h3, h4, p1, p2;

    sscanf(buffer, "PORT %u,%u,%u,%u,%u,%u", &h1, &h2, &h3, &h4, &p1, &p2);

    struct IPPort ip_port = {0};
    snprintf(ip_port.ip, sizeof(ip_port.ip), "%u.%u.%u.%u", h1, h2, h3, h4);
    ip_port.port = (int)(256 * p1 + p2);

    char message[] = "200 PORT command successful.";
    send(client_sock, message, strlen(message), 0);
    return ip_port;
}