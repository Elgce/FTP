#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "const.h"
#include "server_headers/server_funcs.h"

void *client_handler(void *socket_desc) {
    int client_sock = *(int*)socket_desc;
    int login_flag = server_login(client_sock);  // login part
    if (login_flag == 1){
        close(client_sock);
        return NULL;
    }
    // set timeout here, then if socket not receive message for long, close it
    struct timeval timeout;      
    timeout.tv_sec = CONNECT_GAP;
    timeout.tv_usec = 0;
    char ret_message[BUFFER_SIZE];
    if (setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed\n");
    }

    struct TCPConnection file_sock = {0}; // record what port we should use to 
    struct IPPort ipport = {0}; // record which port to be used
    int pasv_fail = 0;
    int rnfr_flag = 0;
    char file_rename[BUFFER_SIZE];
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
        FILE *file = fopen("output.txt", "w"); // 打开文件以写入模式

        if (file != NULL) {
            fprintf(file, "%s", buffer); // 将buffer的内容写入文件
            fclose(file); // 关闭文件
        } else {
            printf("Error: Unable to open file for writing.\n");
        }
        // handle different commands
        if (strncmp(buffer, "PORT", 4) == 0) {
            pasv_fail = 0;
            ipport = server_port(client_sock, buffer);
            if (file_sock.is_active == 1){ // now using some socket
                close(file_sock.sock_fd);
                file_sock.is_active = 0;
            }
        } else if (strncmp(buffer, "PASV", 4) == 0) {
            pasv_fail = 0;
            // cancel former connections
            ipport = server_pasv(client_sock);
            if (file_sock.is_active == 1){ // now using some socket
                close(file_sock.sock_fd);
                file_sock.is_active = 0;
            }
            // build up new connections
            file_sock.sock_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (file_sock.sock_fd == -1){
                pasv_fail = 1;
            }
            file_sock.addr.sin_family = AF_INET;
            file_sock.addr.sin_addr.s_addr = INADDR_ANY;
            file_sock.addr.sin_port = htons(ipport.port);
            if (bind(file_sock.sock_fd, (struct sockaddr *) & file_sock.addr, sizeof(file_sock.addr)) < 0){
                pasv_fail = 1;
            }
            listen(file_sock.sock_fd, 5);
            int c = sizeof(struct sockaddr_in);
            file_sock.sock_fd = accept(file_sock.sock_fd, (struct sockaddr *) & file_sock.addr, (socklen_t*)&c);
            if (file_sock.sock_fd < 0){
                pasv_fail = 1;
            }
            file_sock.is_active = 1;
        } else if (strncmp(buffer, "RETR", 4) == 0 || strncmp(buffer, "STOR", 4) == 0 || strncmp(buffer, "LIST", 4) == 0){
            if (file_sock.is_active == 0){ // if all things ok, means used port
                // build up new connection
                file_sock.sock_fd = socket(AF_INET, SOCK_STREAM, 0);
                if(file_sock.sock_fd == -1){
                    bzero(ret_message, BUFFER_SIZE);
                    strcpy(ret_message, "425 TCP not established.\r\n");
                    send(client_sock, ret_message, strlen(ret_message), 0);
                    continue;
                }
                file_sock.addr.sin_family = AF_INET;
                file_sock.addr.sin_addr.s_addr = INADDR_ANY;
                file_sock.addr.sin_port = htons(ipport.port);
                if (inet_pton(AF_INET, ipport.ip, &file_sock.addr.sin_addr) <= 0){
                    bzero(ret_message, BUFFER_SIZE);
                    strcpy(ret_message, "425 TCP not established.\r\n");
                    send(client_sock, ret_message, strlen(ret_message), 0);
                    continue;
                }
                if (connect(file_sock.sock_fd, (struct sockaddr *)&file_sock.addr, sizeof(file_sock.addr)) < 0){
                    bzero(ret_message, BUFFER_SIZE);
                    strcpy(ret_message, "425 TCP not established.\r\n");
                    send(client_sock, ret_message, strlen(ret_message), 0);
                    continue;
                };
                file_sock.is_active = 1;
            }
            else if (pasv_fail == 1){
                bzero(ret_message, BUFFER_SIZE);
                strcpy(ret_message, "425 TCP not established.\r\n");
                send(client_sock, ret_message, strlen(ret_message), 0);
                continue;
            }
            if (strncmp(buffer, "RETR", 4) == 0){
                server_retr(file_sock.sock_fd, client_sock, buffer);
                file_sock.is_active = 0;
                close(file_sock.sock_fd);
            } else if (strncmp(buffer, "STOR", 4) == 0){
                server_stor(file_sock.sock_fd, client_sock, buffer);
                file_sock.is_active = 0;
                close(file_sock.sock_fd);
            } else if (strncmp(buffer, "LIST", 4) == 0){
                server_list(file_sock.sock_fd, client_sock);
                file_sock.is_active = 0;
                close(file_sock.sock_fd);
            }
        } else if (strncmp(buffer, "SYST", 4) == 0){
            bzero(ret_message, BUFFER_SIZE);
            strcpy(ret_message, "215 UNIX Type: L8\r\n");
            send(client_sock, ret_message, strlen(ret_message), 0);
        } else if (strncmp(buffer, "TYPE I", 6) == 0){
            bzero(ret_message, BUFFER_SIZE);
            strcpy(ret_message, "200 Type set to I.\r\n");
            send(client_sock, ret_message, strlen(ret_message), 0);
        } else if (strncmp(buffer, "ABOR", 4) == 0 || strncmp(buffer, "QUIT", 4) == 0){
            bzero(ret_message, BUFFER_SIZE);
            strcpy(ret_message, "221-Thank you for using the FTP service on ftp.ssast.org.\r\n221 Goodbye.\r\n");
            send(client_sock, ret_message, strlen(ret_message), 0);
            break;
        } else if (strncmp(buffer, "MKD", 3) == 0){
            server_mkd(client_sock, buffer);
        } else if (strncmp(buffer, "RMD", 3) == 0){
            server_rmd(client_sock, buffer);
        } else if (strncmp(buffer, "CWD", 3) == 0){
            server_cwd(client_sock, buffer);
        } else if (strncmp(buffer, "PWD", 3) == 0){
            server_pwd(client_sock);
        } else if (strncmp(buffer, "RNFR", 4) == 0){
            rnfr_flag = server_rnfr(client_sock, buffer);
            bzero(file_rename, BUFFER_SIZE);
            if (rnfr_flag == 1){
                sscanf(buffer, "RNFR %s", file_rename);
            }
        } else if (strncmp(buffer, "RNTO", 4) == 0){
            if (rnfr_flag == 0){
                bzero(ret_message, BUFFER_SIZE);
                strcpy(ret_message, "503 Bad sequence of commands\r\n");
                send(client_sock, ret_message, strlen(ret_message), 0);
                continue;
            }
            server_rnto(client_sock, buffer, file_rename);
            rnfr_flag = 0;
        } else{
            FILE *file = fopen("aoutput.txt", "w"); // 打开文件以写入模式

            if (file != NULL) {
                fprintf(file, "%s", buffer); // 将buffer的内容写入文件
                fclose(file); // 关闭文件
            } else {
                printf("Error: Unable to open file for writing.\n");
            }
        }
        memset(buffer, 0, sizeof(buffer));
    }

    close(client_sock);
    free(socket_desc);
    return NULL;
}

int main(int argc, char* argv[]) {

    int port = 21; // default port

    for (int i = 1; i < argc - 1; i++) { 
        if (strcmp(argv[i], "-port") == 0) {
            port = atoi(argv[i + 1]);
            break;
        }
    }
    char root_file[BUFFER_SIZE] = "/tmp";  

    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-root") == 0) {
            strncpy(root_file, argv[i + 1], sizeof(root_file) - 1);
            root_file[sizeof(root_file) - 1] = '\0';  // 确保字符串结束符存在
            break;
        }
    }

    struct stat st = {0};

    if (stat(root_file, &st) == -1) {  
        if (mkdir(root_file, 0700) != 0) {  
            perror("mkdir error");  
            return 1;
        }
    }

    if (chdir(root_file) != 0) {  
        perror("chdir error");  
        return 1;
    }

    int server_sock, c;
    struct sockaddr_in server, client;
    pthread_t thread_id;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET; // IPv4
    server.sin_addr.s_addr = INADDR_ANY; // from any address
    server.sin_port = htons(port); 

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
        int *new_sock = malloc(sizeof(int));
        *new_sock = client_sock;
        int thrd = pthread_create(&thread_id, NULL, client_handler, (void*)new_sock);
        if (thrd < 0) {
            perror("Could not create thread");
            return 1;
        }
        pthread_detach(thread_id);
    }
    close(server_sock);
    return 0;
}
