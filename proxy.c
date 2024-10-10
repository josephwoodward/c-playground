#include "proxy.h"

#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void *manage_connection(void *args) {
    struct tcp_connection *conn = args;
    ssize_t readVal;
    char buffer[BUFFER_SIZE] = {0};
    int upstream_fd;
    struct sockaddr_in server_addr;
    char server_message[2000], client_message[2000];

    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    // Create socket:
    upstream_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (upstream_fd < 0) {
	printf("Unable to create socket\n");
	/* return -1; */
	return NULL;
    }

    printf("Socket created successfully\n");

    // Set port and IP the same as server-side:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Send connection request to server:
    if (connect(upstream_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
	printf("Unable to connect\n");
	return NULL;
    }

    printf("Connected with server successfully\n");

    // TODO: Do this more efficiently
    while ((readVal = read(conn->client_fd, buffer, BUFFER_SIZE) != -1)) {
	if (send(upstream_fd, buffer, strlen(buffer), 0) < 0) {
	    printf("Unable to send message\n");
	    return NULL;
	}

	while ((readVal = read(upstream_fd, server_message, sizeof(server_message)) != -1)) {
	    send(conn->client_fd, server_message, strlen(server_message), 0);
	}
    }

    return NULL;
}

int start_server(struct tcp_server *s) {
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

    struct tcp_connection clients[MAX_CLIENTS];
    pthread_t thread_id;

    // default to -1 so we can check if set
    memset(clients, -1, sizeof(clients));

    int n;
    while (1) {
	// waiting for connection
	int client_fd = accept(server_socket, NULL, NULL);
	if (client_fd < 0) {
	    perror("failed to connect to client");
	    exit(EXIT_FAILURE);
	}

	// TODO: Add timeout when establishing connection

	// examine existing connections to see if we can establish new ones
	int i;
	for (i = 0; i < MAX_CLIENTS; i++) {
	    struct tcp_connection conn = {
		.client_fd = client_fd,
	    };

	    // if socket is -1 (set by memset above) then we have space available and can accept connection
	    if (clients[i].client_fd == -1) {
		printf("connection accepted...\n");
		clients[i] = conn;
		break;
	    }
	}

	printf("loop index is %d\n", i);
	if (n <= MAX_CLIENTS) {
	    // launch thread listening to socket
	    if (pthread_create(&thread_id, NULL, manage_connection, &clients[i])) {
		perror("failed to create connection thread");
		exit(EXIT_FAILURE);
	    }

	    /* pthread_detach(thread_id); */
	} else {
	    printf("max clients of %d reached", MAX_CLIENTS);
	    close(client_fd);
	}
	n++;
    }

    return 0;
}
