#ifndef PROXY_H
#define PROXY_H

struct http_server {
    int port;
};

struct tcp_connection {
    int client_fd;
    int closed;
};

void *manage_connection(void *args);

int start_server(struct http_server *s);
 
#endif /* PROXY_H */
