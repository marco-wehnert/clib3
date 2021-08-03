#include "tcpmsg.h"
#include "tcpmsg_client.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

#include "config.h"

int TCPMSG_connect_client(tcpmsg_client_vars_t* vars)
{
    struct sockaddr_in serveraddr;
    int result;
    int retval = 0;

    printf(">>> int TCPMSG_connect_client(tcpmsg_client_vars_t* vars)\n");
    vars->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (vars->socket == -1)
    {
        fprintf(stderr, "Socket creation failed\n");
        retval = 1;
    }
    else
    {
        memset(&serveraddr, 0, sizeof(serveraddr));

        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = inet_addr(vars->server_ip);
        serveraddr.sin_port = htons(vars->server_port);

        result = connect(vars->socket,
                (struct sockaddr*) &serveraddr,
                sizeof(serveraddr));

        printf("CLIENT function call connect(...) did return %d\n", result);
        if (result != 0)
        {
            fprintf(stderr, "Connection with server failed\n");
            retval = 2;
        }
    }

    printf("<<< int TCPMSG_connect_client(tcpmsg_client_vars_t* vars)\n");
    return retval;
}

void TCPMSG_disconnect_client(tcpmsg_client_vars_t* self)
{
    close(self->socket);
    self->socket = 0;
}
