// This file define how client acts with file (retr & stor)
#include <stdlib.h>
#include "../server_headers/server_funcs.h"
#include "../const.h"

void server_stor(int client_sock, char* command){
    char file_buffer[BUFFER_SIZE];
    char file_name[BUFFER_SIZE];
    char all_file_name[BUFFER_SIZE];
    sscanf(command, "RETR %s", &file_name);
    snprintf(all_file_name, sizeof(all_file_name), "%s%s", SERVER_SAVE_PATH, file_name);
    FILE* file = fopen(all_file_name, "wb");
    if (file == NULL){
        char message[] = "450 Can't write file.";
        send(client_sock, message, strlen(message), 0);
        return;
    }
    while(1){
        int bytesReceived = recv(client_sock, file_buffer, BUFFER_SIZE, 0);
        if (bytesReceived == 0){
            break;
        }
        else if (bytesReceived < 0){
            char message[] = "426 Connection interrupted.";
            send(client_sock, message, strlen(message), 0);
            return;
        }
        fwrite(file_buffer, 1, bytesReceived, file);
    }
    fclose(file);
    char message[] = "226 Transfer successfully.";
    send(client_sock, message, strlen(message), 0);
    return;
}

void server_retr(int client_sock, char* command){
    // STOR <filename>
    char file_buffer[BUFFER_SIZE];
    char file_name[BUFFER_SIZE];
    char all_file_name[BUFFER_SIZE];
    char server_response[BUFFER_SIZE];
    sscanf(command, "STOR %s", &file_name);
    snprintf(all_file_name, sizeof(all_file_name), "%s%s", CLIENT_SAVE_PATH, file_name);
    printf("file name: %s", all_file_name);
    FILE* file = fopen(all_file_name, "rb");
    if (file == NULL){
        char message[] = "451 Can't read file.";
        send(client_sock, message, strlen(message), 0);
        return;
    }
    while(1){
        size_t bytesRead = fread(file_buffer, 1, BUFFER_SIZE, file);
        if (bytesRead == 0){
            break;
        }
        else if (bytesRead < 0){
            char message[] = "426 Connection interrupted.";
            send(client_sock, message, strlen(message), 0);
            return;
        }
        send(client_sock, file_buffer, bytesRead, 0);
    }
    fclose(file);
    char message[] = "226 Transfer successfully.";
    send(client_sock, message, strlen(message), 0);
    return;
}