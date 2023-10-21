// This file define how client acts with RETR
#include "../client_headers/client_funcs.h"
#include "../const.h"

int client_retr(int client_sock, char* command){
    send(client_sock, command, strlen(command), 0);
    printf("debuggg\r\n");
    return 0;
}