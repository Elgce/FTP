// This file define how server acts when pasv
#include <string.h>
#include "../client_headers/client_funcs.h"
#include "../const.h"
static int pasv_port = 20002;
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