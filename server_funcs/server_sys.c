// This file define how server handle system calls
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "../server_headers/server_funcs.h"
#include "../const.h"

void server_list(int client_sock, int msg_sock){
    FILE *fp;
    char message[BUFFER_SIZE];
    char cmd[BUFFER_SIZE];
    char line[BUFFER_SIZE];
    bzero(message, BUFFER_SIZE);
    strcpy(message, "150 Openning data channel for directory list.\r\n");
    send(msg_sock, message, strlen(message), 0);
    
    snprintf(cmd, sizeof(cmd), "ls -l");
    fp = popen(cmd, "r");
    if (fp == NULL){
        bzero(message, BUFFER_SIZE);
        strcpy(message, "451 Requested action aborted.\r\n");
        send(msg_sock, message, strlen(message), 0);
        return;
    }
    bzero(message, BUFFER_SIZE);
    while (fgets(line, sizeof(line), fp) != NULL){
        strncat(message, line, BUFFER_SIZE - strlen(message) - 1);
        // strncat(message, "\r\n", BUFFER_SIZE - strlen(message) - 1);
    }
    strncat(message, "\r\n", BUFFER_SIZE - strlen(message) - 1);
    send(client_sock, message, strlen(message), 0);
    pclose(fp);
    bzero(message, BUFFER_SIZE);
    strcpy(message, "226 Transfer successfully.\r\n");
    send(msg_sock, message, strlen(message), 0);
    return;
}

void server_mkd(int client_sock, char* command){
    char dir_name[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    sscanf(command, "MKD %s", dir_name);
    if (mkdir(dir_name, 0777) != 0){
        bzero(message, BUFFER_SIZE);
        strcpy(message, "550 Action not taken\r\n");
        send(client_sock, message, strlen(message), 0);
    }
    bzero(message, BUFFER_SIZE);
    strcpy(message, "257 Directory created successfully\r\n");
    send(client_sock, message, strlen(message), 0);
    return;
}

void server_rmd(int client_sock, char* command){
    char dir_name[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    sscanf(command, "MKD %s", dir_name);
    if (rmdir(dir_name) != 0){
        bzero(message, BUFFER_SIZE);
        strcpy(message, "550 Action not taken\r\n");
        send(client_sock, message, strlen(message), 0);
    }
    bzero(message, BUFFER_SIZE);
    strcpy(message, "257 Directory created successfully\r\n");
    send(client_sock, message, strlen(message), 0);
    return;
}

void server_pwd(int client_sock){
    char path[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    if(getcwd(path, BUFFER_SIZE) == NULL){
        bzero(message, BUFFER_SIZE);
        strcpy(message, "421 Service not Accessible\r\n");
        send(client_sock, message, strlen(message), 0);
        return;
    }
    bzero(message, BUFFER_SIZE);
    snprintf(message, sizeof(message), "257 \"%s\" is current directory\r\n", path);
    send(client_sock, message, strlen(message), 0);
    return;
}

void server_cwd(int client_sock, char* command){
    char path[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    sscanf(command, "CWD %s", path);
    
    if(chdir(path) != 0){
        bzero(message, BUFFER_SIZE);
        strcpy(message, "421 Service not Accessible\r\n");
        send(client_sock, message, strlen(message), 0);
        return;
    }
    snprintf(message, sizeof(message), "257 \"%s\" is current directory\r\n", path);
    send(client_sock, message, strlen(message), 0);
    return;
}

int server_rnfr(int client_sock, char* command){
    char file_name[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    sscanf(command, "RNFR %s", file_name);
    if (access(file_name, F_OK) != 0){
        snprintf(message, sizeof(message), "550 File not found.\r\n");
        send(client_sock, message, strlen(message), 0);
        return 0;
    }
    snprintf(message, sizeof(message), "350 File/folder exists, ready for destination name.\r\n");
    send(client_sock, message, strlen(message), 0);
    return 1;
}

void server_rnto(int client_sock, char* command, char* source_name){
    char file_name[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    sscanf(command, "RNTO %s", file_name);
    if (rename(source_name, file_name) == 0){
        snprintf(message, sizeof(message), "250 File/folder successfully renamed\r\n");
        send(client_sock, message, strlen(message), 0);
        return;
    } else if (errno == EEXIST){
        snprintf(message, sizeof(message), "553 Requested action not taken. File name not allowed.\r\n");
        send(client_sock, message, strlen(message), 0);
        return;
    } else {
        snprintf(message, sizeof(message), "550 Rename failed due to insufficient permissions.\r\n");
        send(client_sock, message, strlen(message), 0);
        return;
    }
    
}