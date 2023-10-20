#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    // 这里可以放置客户端通信的代码，例如读/写操作
    // ...
    // 用完socket后不要忘记关闭
    close(client_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // ... (创建socket，绑定到端口，然后开始监听)

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *) & address, (socklen_t*) & addrlen)
        if (new_socket < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        pid_t child_pid = fork();  // new process
        if (child_pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (child_pid == 0) {  // child process
            close(server_fd); 
            handle_client(new_socket); 
            exit(0);  
        } else {  // parent process
            close(new_socket);
        }
    }

    close(server_fd);
    return 0;
}
