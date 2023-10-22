CC = gcc
CFLAGS = -Wall -I. -Iserver_headers -Iclient_headers

SERVER_SRCS = server.c $(wildcard server_funcs/*.c)
SERVER_OBJS = $(SERVER_SRCS:.c=.o)
SERVER_TARGET = server_file/server

CLIENT_SRCS = client.c $(wildcard client_funcs/*.c)
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
CLIENT_TARGET = client_file/client

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_OBJS) $(CLIENT_OBJS) $(SERVER_TARGET) $(CLIENT_TARGET)
