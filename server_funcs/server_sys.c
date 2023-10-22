// This file define how server handle system calls
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "../server_headers/server_funcs.h"
#include "../const.h"

void server_list(int client_sock, int msg_sock){
    FILE *fp;
    char file_name[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char cmd[BUFFER_SIZE];
    char line[BUFFER_SIZE];
    bzero(message, BUFFER_SIZE);
    strcpy(message, "150 Openning data channel for directory list.");
    send(msg_sock, message, strlen(message), 0);
    
    strcpy(file_name, SERVER_SAVE_PATH);
    snprintf(cmd, sizeof(cmd), "ls -l %s", file_name);
    fp = popen(cmd, "r");
    if (fp == NULL){
        bzero(message, BUFFER_SIZE);
        strcpy(message, "451 Requested action aborted.");
        send(msg_sock, message, strlen(message), 0);
        return;
    }
    bzero(message, BUFFER_SIZE);
    while (fgets(line, sizeof(line), fp) != NULL){
        strncat(message, line, BUFFER_SIZE - strlen(message) - 1);
        // strncat(message, "\r\n", BUFFER_SIZE - strlen(message) - 1);
    }
    send(client_sock, message, strlen(message), 0);
    pclose(fp);
    bzero(message, BUFFER_SIZE);
    strcpy(message, "226 Transfer successfully.");
    send(msg_sock, message, strlen(message), 0);
    return;
}