// This file define how client acts with file (retr & stor)
#include <stdlib.h>
#include "../server_headers/server_funcs.h"
#include "../const.h"

void server_stor(int client_sock, int msg_sock, char* command){
    char file_buffer[BUFFER_SIZE];
    char file_name[BUFFER_SIZE];
    char message[BUFFER_SIZE];

    bzero(message, BUFFER_SIZE);
    strcpy(message, "150 Begin transfer");
    send(msg_sock, message, strlen(message), 0);

    sscanf(command, "STOR %s", &file_name);
    FILE* file = fopen(file_name, "wb");
    if (file == NULL){
        bzero(message, BUFFER_SIZE);
        strcpy(message, "450 Can't write file.");
        send(msg_sock, message, strlen(message), 0);
        return;
    }
    while(1){
        int bytesReceived = recv(client_sock, file_buffer, BUFFER_SIZE, 0);
        if (bytesReceived == 0){
            break;
        }
        else if (bytesReceived < 0){
            bzero(message, BUFFER_SIZE);
            strcpy(message, "426 Connection interrupted.");
            send(msg_sock, message, strlen(message), 0);
            fclose(file);
            return;
        }
        
        fwrite(file_buffer, 1, bytesReceived, file);
        if (bytesReceived < BUFFER_SIZE){
            break;
        }
    }
    fclose(file);
    bzero(message, BUFFER_SIZE);
    strcpy(message, "226 Transfer successfully.");
    send(msg_sock, message, strlen(message), 0);
    return;
}

void server_retr(int client_sock, int msg_sock, char* command){
    // STOR <filename>
    char file_buffer[BUFFER_SIZE];
    char file_name[BUFFER_SIZE];
    char message[BUFFER_SIZE];

    bzero(message, BUFFER_SIZE);
    strcpy(message, "150 Begin transfer");
    send(msg_sock, message, strlen(message), 0);

    sscanf(command, "RETR %s", &file_name);
    FILE* file = fopen(file_name, "rb");
    if (file == NULL){
        bzero(message, BUFFER_SIZE);
        strcpy(message, "451 Can't read file.");
        send(msg_sock, message, strlen(message), 0);
        return;
    }
    while(1){
        size_t bytesRead = fread(file_buffer, 1, BUFFER_SIZE, file);
        if (bytesRead == 0){
            break;
        }
        else if (bytesRead < 0){
            bzero(message, BUFFER_SIZE);
            strcpy(message, "426 Connection interrupted.");
            send(msg_sock, message, strlen(message), 0);
            fclose(file);
            return;
        }
        send(client_sock, file_buffer, bytesRead, 0);
    }
    fclose(file);
    bzero(message, BUFFER_SIZE);
    strcpy(message, "226 Transfer successfully.");
    send(msg_sock, message, strlen(message), 0);
    return;
}