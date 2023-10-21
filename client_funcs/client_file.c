// This file define how client acts with file (retr & stor)
#include <stdlib.h>
#include "../client_headers/client_funcs.h"
#include "../const.h"

void client_retr(int client_sock, char* command){ // ask from server
    // RETR <filename>
    char file_buffer[BUFFER_SIZE];
    char file_name[BUFFER_SIZE];
    char all_file_name[BUFFER_SIZE];
    char server_response[BUFFER_SIZE];
    sscanf(command, "RETR %s", &file_name);
    snprintf(all_file_name, sizeof(all_file_name), "%s%s", CLIENT_SAVE_PATH, file_name);

    FILE* file = fopen(all_file_name, "wb");
    if (file == NULL){
        printf("Failed to write %s\r\n", all_file_name);
        return;
    }
    while(1){
        int bytesReceived = recv(client_sock, file_buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0){
            break;
        }
        fwrite(file_buffer, 1, bytesReceived, file);
    }
    fclose(file);
    recv(client_sock, server_response, sizeof(server_response), 0);
    printf("%s\r\n", server_response);
    return;
}

void client_stor(int client_sock, char* command){ // send to server
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
        printf("Failed to read %s\r\n", all_file_name);
        return;
    }
    while(1){
        size_t bytesRead = fread(file_buffer, 1, BUFFER_SIZE, file);
        if (bytesRead <= 0){
            break;
        }
        send(client_sock, file_buffer, bytesRead, 0);
    }
    fclose(file);
    recv(client_sock, server_response, sizeof(server_response), 0);
    printf("%s\r\n", server_response);
    return;
}