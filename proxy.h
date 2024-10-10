#ifndef PROXY_H
#define PROXY_H

#ifndef MAX_CLIENTS
#define MAX_CLIENTS 8
#endif

struct tcp_server {
    int port;
};

struct tcp_connection {
    int client_fd;
    int closed;
};

void *manage_connection(void *args);

int start_server(struct tcp_server *s);
 
#endif /* PROXY_H */
