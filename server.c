#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <errno.h>
#include "const.h"
#include "server_headers/server_login.h"

void *client_handler(void *socket_desc) {
    int client_sock = *(int*)socket_desc;
    
    int login_flag = handle_login(client_sock);  // login part
    if (login_flag == 1){
        close(client_sock);
        return;
    }

    // set timeout here, then if socket not receive message for long, close it
    struct timeval timeout;      
    timeout.tv_sec = CONNECT_GAP;
    timeout.tv_usec = 0;

    if (setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed\n");
    }

    // 持续监听客户端命令
    while(1) {
        char buffer[BUFFER_SIZE];
        int read_size = recv(client_sock, buffer, sizeof(buffer), 0);

        if(read_size <= 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                puts("Client timeout. Connection closed");
            } else {
                puts("Client disconnected or other error.");
            }
            fflush(stdout);
            break;
        }

        // 根据接收到的命令调用对应的功能函数
        if (strncmp(buffer, "LIST", 4) == 0) {
            // handle_list(client_sock);
        } else if (strncmp(buffer, "RETRIEVE", 8) == 0) {
            // handle_retrieve(client_sock);
        } else if (strncmp(buffer, "HAHA", 4) == 0){
            //
        }

        memset(buffer, 0, sizeof(buffer));  // 清空缓冲区
    }

    close(client_sock);
    return;
}

int main() {
    int server_sock, client_sock, c;
    struct sockaddr_in server, client;
    pthread_t thread_id;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET; // IPv4
    server.sin_addr.s_addr = INADDR_ANY; // from any address
    server.sin_port = htons(SERVER_PORT); 

    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    listen(server_sock, 5);
    c = sizeof(struct sockaddr_in);

    while (1) {
        int client_sock = accept(server_sock, (struct sockaddr *) & client, (socklen_t*)&c);
        if (client_sock < 0){
            perror("Accept failed");
            return 1;
        }
        else{
            printf("One new connection created.\r\n");
        }
        int *new_sock = malloc(1);
        *new_sock = client_sock;
        int thrd = pthread_create(&thread_id, NULL, client_handler, (void*)new_sock);
        if (thrd < 0) {
            perror("Could not create thread");
            return 1;
        }
        pthread_detach(thread_id);
    }

    return 0;
}
