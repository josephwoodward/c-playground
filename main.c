#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CLIENTS 3

#include "proxy.c"
#include "proxy.h"

int main(int argc, char *argv[]) {
    struct http_server s = {.port = 8086};
    start_server(&s);

    return 0;
}
