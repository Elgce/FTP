#ifndef CLIENT_FUNCS_H
#define CLIENT_FUNCS_H

#include <stdio.h>
#include <string.h>

int client_login(int client_sock);
int client_port(int client_sock, char* command);

#endif