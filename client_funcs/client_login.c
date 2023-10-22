// This file define how client acts to login server
#include "../client_headers/client_funcs.h"
#include "../const.h"

int client_login(int client_sock) {
    char buffer[BUFFER_SIZE];
    int read_size;

    read_size = recv(client_sock, buffer, sizeof(buffer), 0);
    if(read_size > 0) {
        printf("%s", buffer);
        memset(buffer, 0, read_size);
    }
    printf("Enter the username you use to login: USER ");
    fgets(buffer, sizeof(buffer), stdin);
    char* clean_string = strtok(buffer, "\n\r");
    char user[BUFFER_SIZE];
    snprintf(user, sizeof(user), "USER %s\r\n", clean_string);
    send(client_sock, user, strlen(user), 0);

    read_size = recv(client_sock, buffer, sizeof(buffer), 0);
    if(read_size > 0){
        printf("%s", buffer);
        if (strncmp(buffer, "331", 3) != 0){
            return 1;
        }
        memset(buffer, 0, read_size);
    } else{
        return 1;
    }

    printf("Enter email as password: PASS ");
    fgets(buffer, sizeof(buffer), stdin);
    char cmd[BUFFER_SIZE];
    snprintf(cmd, sizeof(cmd), "PASS %s\r\n", buffer);
    send(client_sock, cmd, strlen(cmd), 0);
    bzero(buffer, sizeof(buffer));
    read_size = recv(client_sock, buffer, sizeof(buffer), 0);
    if(read_size > 0) {
        printf("%s", buffer);
        if (strncmp(buffer, "230", 3) != 0){
            return 1;
        }
    }
    else{
        return 1;
    }
    return 0;
}
