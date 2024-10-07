#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 3

typedef struct {
  int port;
} http_server;


typedef struct {
  int client_fd;
} tcp_connection;

void* makeConnection(void *args){
  tcp_connection *conn = args;
  ssize_t readVal;
  char buffer[BUFFER_SIZE] = {0};

    while ((readVal = read(conn->client_fd, buffer, BUFFER_SIZE))) {
      printf("client: %s", buffer);
      /* break; */
    }
  return NULL;
}

int start_server(http_server *s) {
  struct sockaddr_in address = {
      .sin_family = AF_INET,
      .sin_port = htons(s->port),
      .sin_addr.s_addr = INADDR_ANY,
  };

  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  printf("starting server on port %d\n", s->port);

  // Bind server_socket to address
  if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("failed to bind to socket");
    exit(EXIT_FAILURE);
  }

  // Allow n clients to be queued while the server processes
  if (listen(server_socket, 2) < 0) {
    perror("listener failed");
    exit(EXIT_FAILURE);
  }

  printf("Server started and listening on port %d\n", s->port);

  // Wait for client to connect, then open a socket
  int client_fd = accept(server_socket, NULL, NULL);
  if (client_fd < 0) {
    perror("failed to connect to client");
    exit(EXIT_FAILURE);
  }

  //TODO: Look into adding timeouts

  printf("connection accepted\n");

  // TODO: track multiple connections
  pthread_t thread_id;
  tcp_connection active_conn = {
      .client_fd = client_fd,
  };

  int established = 0;

  while (1) {
     
    if (established == 0) {
      pthread_create(&thread_id, NULL, makeConnection, &active_conn);
      established = 1;
    }

    /* char message[] = "HTTP/1.1 200 OK\r\n\r\n<html>Hello World!</html>\r\n\r\n"; */
    /* send(client_fd, message, strlen(message), 0); */
  }

  return 0;
}



int main(void) {
  http_server s = {.port = 8085};
  start_server(&s);

  return 0;
}
