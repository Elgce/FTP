#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// define const int
#define BUFFER_SIZE 1024
#define CONNECT_GAP 100000
#define SERVER_PORT 8083

// define const string
#define SERVER_IP   "127.0.0.1"
#define SERVER_NAME "ftp.ssast.org"
#define SERVER_SAVE_PATH "./server_file/"
#define CLIENT_SAVE_PATH "./client_file/"

// define struct
typedef struct TCPConnection{
    struct sockaddr_in addr;
    int sock_fd; 
    int is_active; // show this tcp is active or not;
};

typedef struct IPPort{
    char ip[20];
    int port;
}; // store ip:port to be used

#endif
