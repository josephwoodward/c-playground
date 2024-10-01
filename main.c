#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
  int port;
} http_server;

int start_server(http_server *s) {
  /* struct sockaddr_in address; */
  /* address.sin_family = AF_INET; */
  /* address.sin_port = htons(s->port); // Port 8080 */
  /* address.sin_addr.s_addr = INADDR_ANY; */

  struct sockaddr_in address = {
      .sin_family = AF_INET,
      .sin_port = s->port,
      .sin_addr.s_addr = INADDR_ANY,
  };

  int server_socket = socket(AF_INET, SOCK_STREAM, 0);

  // Bind server_socket to address
  bind(server_socket, (struct sockaddr *)&address, sizeof(address));
  listen(server_socket, 4);

  // Listen for clients and allow the accept function to be used
  // Allow 4 clients to be queued while the server processes

  // Wait for client to connect, then open a socket
  int client_socket = accept(server_socket, NULL, NULL);

  char message[] = "Hello, World!";

  // Send message to the client
  send(client_socket, message, strlen(message), 0);

  // Close the client socket
  close(client_socket);

  return 0;
}

int main(void) {

  http_server s = {.port = 8080};
  start_server(&s);

  return 0;
}
