#include "tcpmsg.h"
#include "tcpmsg_server.h"
#include "utils.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "config.h"

int TCPMSG_create_server(tcpmsg_server_vars_t* vars)
{
    struct sockaddr_in serveraddr;
    int result;
    
    printf(">>> int TCPMSG_create_server(tcpmsg_server_vars_t* vars)\n");
    printf("SERVER port: %d\n", vars->port);

    vars->socket = socket(AF_INET, SOCK_STREAM, 0);
    printf("SERVER function call socket(...) did return %d\n", vars->socket);
    if (vars->socket == -1)
    {
        fprintf(stderr, "SERVER socket creation failed\n");
        printf("<<< int TCPMSG_create_server(tcpmsg_server_vars_t* vars)\n");
        return 1;
    }
    memset(&serveraddr, 0, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(vars->port);

    result = bind(vars->socket, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
    printf("SERVER function call bind(...) did return %d\n", result);
    if (result != 0)
    {
        fprintf(stderr, "SERVER bind socket failed\n");
        printf("<<< int TCPMSG_create_server(tcpmsg_server_vars_t* vars)\n");
        return 2;
    }

	pthread_create((pthread_t*) &vars->listen_thread, NULL, TCPMSG_listen_thread, vars);
    printf("<<< int TCPMSG_create_server(tcpmsg_server_vars_t* vars)\n");
    return 0;
}

void TCPMSG_shutdown_server(tcpmsg_server_vars_t* self) 
{
    void* result;
    
    printf(">>> void TCPMSG_shutdown_server(tcpmsg_server_vars_t* self)\n");
    
    if (self->listen_thread)
    {
        printf("SERVER call function pthread_cancel\n");
        pthread_cancel(self->listen_thread);
        printf("SERVER call function pthread_join(...)\n");
        pthread_join(self->listen_thread, &result);
        if (result == PTHREAD_CANCELED)
        {
            printf("SERVER listen thread had been cancelled\n");
        }
        self->listen_thread = 0;
    }
    close(self->socket);
    self->socket = 0;

    printf("<<< void TCPMSG_shutdown_server(tcpmsg_server_vars_t* self)\n");
}

void* TCPMSG_listen_thread(void* ptr)
{
    int result;
    unsigned int len;
    struct sockaddr_in clientaddr;
    //tcpmsg_connection_vars_t conn_vars;
    int conn_socket;
    bool run = true;
    
    printf(">>> void* TCPMSG_listen_thread(void* ptr)\n");
    if (ptr == NULL)
    {
        fprintf(stderr, "SERVER illegal argument value: ptr == NULL\n");
        printf("<<< void* TCPMSG_listen_thread(void* ptr)\n");
        return (void*)0;
    }
    
    tcpmsg_server_vars_t* vars = (tcpmsg_server_vars_t*) ptr;
    printf("SERVER listen thread will listen on socket handle %d\n", vars->socket);
    while (run)
    {
        errno = 0;
        result = listen(vars->socket, 5);
        printf("SERVER function call listen(...) did return %d\n", result);
        if (errno) handle_error(errno);
        if (result != 0)
        {
            printf("<<< void* TCPMSG_listen_thread(void* ptr)\n");
            return (void*)1;
        }

        conn_socket = accept(vars->socket, (struct sockaddr*) &clientaddr, &len);
        printf("SERVER function call accept(...) did return %d\n", conn_socket);
    }
    printf("<<< void* TCPMSG_listen_thread(void* ptr)\n");

    return (void*)0;
}
