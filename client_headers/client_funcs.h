#ifndef CLIENT_FUNCS_H
#define CLIENT_FUNCS_H

#include <stdio.h>
#include <string.h>

int client_login(int client_sock);
struct IPPort client_port(int client_sock, char* command);
struct IPPort client_pasv(int client_sock, char* command);
void client_retr(int client_sock, char* command);
void client_stor(int client_sock, char* command);

#endif