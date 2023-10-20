#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"  // IP
#define PORT 52001               // port
#define BUFFER_SIZE 1024       // buffer size

void login(int client_fd){
    char buffer[BUFFER_SIZE] = {0};
    recv(client_fd, buffer, sizeof(buffer), 0);
    printf("%s", buffer);

    // send user name to server
    send(client_fd, "USER anonymous\r\n", strlen("USER anonymous\r\n"), 0);
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_fd, buffer, sizeof(buffer), 0);
    printf("%s", buffer);

    // enter email as pwd
    printf("Enter your email as password: ");
    fgets(email, BUFFER_SIZE, stdin);
    snprintf(buffer, sizeof(buffer), "PASS %s", email);
    send(client_fd, buffer, strlen(buffer), 0);
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_fd, buffer, sizeof(buffer), 0);
    printf("%s", buffer);
}

int main() {
    struct sockaddr_in address;
    int client_fd = 0;
    char email[BUFFER_SIZE];

    // create Socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET; // IPv4
    address.sin_port = htons(PORT);
    
    if(inet_pton(AF_INET, SERVER_IP, &address.sin_addr) <= 0) {
        perror("Invalid address or Address not supported");
        exit(EXIT_FAILURE);
    }

    // connect
    if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }


    // 关闭连接
    close(client_fd);

    return 0;
}
