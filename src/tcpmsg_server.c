#include "tcpmsg.h"
#include "tcpmsg_server.h"
#include "utils.h"
#include "linked_list.h"
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
#include <netinet/tcp.h>

#include "config.h"

int TCPMSG_create_server(tcpmsg_server_vars_t* self)
{
    struct sockaddr_in serveraddr;
    int enable = 1;
    int result;
//    printf(">>> int TCPMSG_create_server(tcpmsg_server_vars_t* self)\n");

//    printf("SERVER port: %d\n", self->port);
    self->socket = socket(AF_INET, SOCK_STREAM, 0);
//    printf("SERVER function call socket(...) did return %d\n", self->socket);
    if (self->socket == -1)
    {
        fprintf(stderr, "SERVER socket creation failed\n");
//        printf("<<< int TCPMSG_create_server(tcpmsg_server_vars_t* self)\n");
        return 1;
    }
    //setsockopt(self->socket, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(reusePort));
    if (setsockopt(self->socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
        fprintf(stderr, "setsockopt(SO_REUSEADDR) failed");

    if (setsockopt(self->socket, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable)) < 0)
        fprintf(stderr, "setsockopt(TCP_NODELAY) failed");

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(self->port);

    result = bind(self->socket, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
//    printf("SERVER function call bind(...) did return %d\n", result);
    if (result != 0)
    {
        fprintf(stderr, "SERVER bind socket failed: ");
        perror(NULL);
//        printf("<<< int TCPMSG_create_server(tcpmsg_server_vars_t* self)\n");
        return 2;
    }

	pthread_create((pthread_t*) &self->listen_thread_id, NULL, TCPMSG_listen_thread, self);
    // We need to wait until server is ready to listen for
    // new connections.
    usleep(10000);
//    printf("<<< int TCPMSG_create_server(tcpmsg_server_vars_t* self)\n");
    return 0;
}

void TCPMSG_shutdown_server(tcpmsg_server_vars_t* self)
{
    void* result;

    //printf(">>> void TCPMSG_shutdown_server(tcpmsg_server_vars_t* self)\n");

    if (self->listen_thread_id)
    {
        //printf("SERVER call function pthread_cancel\n");
        pthread_cancel(self->listen_thread_id);
        //printf("SERVER call function pthread_join(...)\n");
        pthread_join(self->listen_thread_id, &result);
//        if (result == PTHREAD_CANCELED)
//        {
//            printf("SERVER listen thread had been cancelled\n");
//        }
        self->listen_thread_id = 0;
    }
    close(self->socket);
    self->socket = 0;

    TCPMSG_close_all_server_connections(self);

//    printf("<<< void TCPMSG_shutdown_server(tcpmsg_server_vars_t* self)\n");
}

void TCPMSG_close_all_server_connections(tcpmsg_server_vars_t* self)
{
    void* result;
    list_element_t* list_element;
    tcpmsg_reader_vars_t* connection;
    //printf("Clean up connection list\n");
    //printf("Number of connections in list: %d\n", self->connections.count);
    list_element = self->connections.front;
    while (list_element)
    {
        connection = list_element->object;
        if (!connection->finished)
        {
            pthread_cancel(connection->reader_thread_id);
        }
        pthread_join(connection->reader_thread_id, &result);
        free(connection);
        list_element = list_element->next;
    }

    //printf("Call function ll_clear(...)\n");
    ll_clear(&self->connections);

}

void* TCPMSG_listen_thread(void* ptr)
{
    int result;
    unsigned int len;
    struct sockaddr_in clientaddr;
    int conn_socket;
    bool run = true;

//    printf("    >>> void* TCPMSG_listen_thread(void* ptr)\n");
    if (ptr == NULL)
    {
//        fprintf(stderr, "SERVER illegal argument value: ptr == NULL\n");
//        printf("<<< void* TCPMSG_listen_thread(void* ptr)\n");
        return (void*)0;
    }

    tcpmsg_server_vars_t* self = (tcpmsg_server_vars_t*) ptr;
//    printf("SERVER listen thread will listen on socket handle %d\n", self->socket);
    while (run)
    {
        result = listen(self->socket, 5);
//        printf("SERVER function call listen(...) did return %d\n", result);
        if (errno) perror("listen");
        if (result != 0)
        {
  //          printf("<<< void* TCPMSG_listen_thread(void* ptr)\n");
            return (void*)1;
        }

        conn_socket = accept(self->socket, (struct sockaddr*) &clientaddr, &len);
//        printf("SERVER function call accept(...) did return %d\n", conn_socket);
        tcpmsg_reader_vars_t* connection =
            calloc(sizeof(tcpmsg_reader_vars_t), 1);
//        printf("SERVER memory address for new connection = %p\n", connection);
        ll_push_back(&self->connections, connection);
        connection->socket = conn_socket;
        connection->finished = false;
        pthread_create((pthread_t*) &connection->reader_thread_id, NULL, TCPMSG_reader_thread, connection);

    }
//    printf("<<< void* TCPMSG_listen_thread(void* ptr)\n");

    return (void*)0;
}
