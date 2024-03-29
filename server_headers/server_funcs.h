// define functions for use of server_login
#ifndef SERVER_FUNCS_H
#define SERVER_FUNCS_H

#include <stdio.h>
#include <string.h>
#include "../const.h"

// declare functions
int server_login(int client_sock);
struct IPPort server_port(int client_sock, char* buffer);
struct IPPort server_pasv(int client_sock);
void server_retr(int client_sock, int msg_sock, char* command);
void server_stor(int client_sock, int msg_sock, char* command);
void server_list(int client_sock, int msg_sock);
void server_mkd(int client_sock, char* command);
void server_rmd(int client_sock, char* command);
void server_pwd(int client_sock);
void server_cwd(int client_sock, char* command);
int server_rnfr(int client_sock, char* command);
void server_rnto(int client_sock, char* command, char* source_name);

#endif
