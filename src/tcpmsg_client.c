#include "tcpmsg.h"
#include "tcpmsg_client.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/tcp.h>

#include "config.h"

int TCPMSG_connect_client(tcpmsg_client_vars_t* self)
{
    struct sockaddr_in serveraddr;
    int result;
    int enable = 1;

    self->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (self->socket == -1)
    {
        fprintf(stderr, "Socket creation failed: ");
        perror(NULL);
        return -1;
    }

    if (setsockopt(self->socket, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable)) < 0)
        fprintf(stderr, "setsockopt(TCP_NODELAY) failed");

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(self->server_ip);
    serveraddr.sin_port = htons(self->server_port);

    result = connect(self->socket,
            (struct sockaddr*) &serveraddr,
            sizeof(serveraddr));

    if (result != 0)
    {
        fprintf(stderr, "Connect to server failed: ");
        perror(NULL);
        return -2;
    }

    self->reader_vars.socket = self->socket;
    pthread_create((pthread_t*) &self->reader_vars.reader_thread_id, NULL, TCPMSG_reader_thread, &self->reader_vars);

    return 0;
}

void TCPMSG_disconnect_client(tcpmsg_client_vars_t* self)
{
    void* res;
    close(self->socket);
    self->socket = 0;

    if (!self->reader_vars.finished)
    {
        pthread_cancel(self->reader_vars.reader_thread_id);
    }
    pthread_join(self->reader_vars.reader_thread_id, &res);
}
