#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

typedef struct {
  int port;
} http_server;

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

  printf("connection accepted\n");

  char buffer[BUFFER_SIZE] = {0};
  ssize_t readVal;
  while (1) {
    while ((readVal = read(client_fd, buffer, BUFFER_SIZE))) {
      /* printf("client: %s", buffer); */
      break;
    }

    char message[] = "HTTP/1.1 200 OK\r\n\r\n<html>Hello World!</html>\r\n\r\n";
    send(client_fd, message, strlen(message), 0);
  }

  return 0;
}

int main(void) {
  http_server s = {.port = 8085};
  start_server(&s);

  return 0;
}
