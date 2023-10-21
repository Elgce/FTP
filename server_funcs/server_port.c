// This file define servers' action to port command
#include "../server_headers/server_funcs.h"
#include "../const.h"

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