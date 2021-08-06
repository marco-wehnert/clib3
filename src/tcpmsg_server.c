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
    self->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (self->socket == -1)
    {
        fprintf(stderr, "SERVER socket creation failed\n");
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
    if (result != 0)
    {
        fprintf(stderr, "SERVER bind socket failed: ");
        perror(NULL);
        return 2;
    }

	pthread_create((pthread_t*) &self->listen_thread_id, NULL, TCPMSG_listen_thread, self);
    usleep(10000);
    return 0;
}

void TCPMSG_shutdown_server(tcpmsg_server_vars_t* self)
{
    void* result;


    if (self->listen_thread_id)
    {
        pthread_cancel(self->listen_thread_id);
        pthread_join(self->listen_thread_id, &result);
        self->listen_thread_id = 0;
    }
    close(self->socket);
    self->socket = 0;

    TCPMSG_close_all_server_connections(self);

}

void TCPMSG_close_all_server_connections(tcpmsg_server_vars_t* self)
{
    void* result;
    list_element_t* list_element;
    tcpmsg_reader_vars_t* connection;
    list_element = self->connections.front;
    while (list_element)
    {
        connection = list_element->object;
        if (!connection->finished)
        {
            printf("Cancel reader thread with id %ld\n",
                    connection->reader_thread_id);

            pthread_cancel(connection->reader_thread_id);
        }
        pthread_join(connection->reader_thread_id, &result);
        free(connection);
        list_element = list_element->next;
    }

    ll_clear(&self->connections);

}

bool reader_has_finished(void* ptr)
{
    tcpmsg_reader_vars_t* reader = ptr;
    return reader->finished;
}

void TCPMSG_remove_finished_threads(tcpmsg_server_vars_t* server_vars)
{
    linked_list_t* p_conn_list = &server_vars->connections;
    linked_list_t  finished_conn_list;
    list_element_t* p_element = NULL;
    tcpmsg_reader_vars_t* p_reader = NULL;
    void* result;

    memset(&finished_conn_list, 0, sizeof(linked_list_t));

    // 1. Collect all readers which finished
    p_element = p_conn_list->front;
    while (p_element != NULL)
    {
        p_reader = p_element->object;
        if (p_reader->finished)
        {
            ll_push_back(&finished_conn_list, p_reader);
        }
        p_element = p_element->next;
    }

    // 2. Remove all finished readers from connection list
    ll_remove_all(p_conn_list, &reader_has_finished);

    // 3. join threads and delete reader object
    p_element = finished_conn_list.front;
    while (p_element != NULL)
    {
        p_reader = p_element->object;
        pthread_join(p_reader->reader_thread_id, &result);
        free(p_reader);
        p_element = p_element->next;
    }

    ll_clear(&finished_conn_list);
}


void* TCPMSG_listen_thread(void* ptr)
{
    int result;
    unsigned int len;
    struct sockaddr_in clientaddr;
    int conn_socket;

    tcpmsg_server_vars_t* self = (tcpmsg_server_vars_t*) ptr;
    printf("Started listen thread with id %ld\n", self->listen_thread_id);
    while (1)
    {
        result = listen(self->socket, 5);
        if (result != 0)
        {
            perror("listen");
            return (void*)1;
        }

        conn_socket = accept(self->socket, (struct sockaddr*) &clientaddr, &len);
        tcpmsg_reader_vars_t* connection =
            calloc(sizeof(tcpmsg_reader_vars_t), 1);
        ll_push_back(&self->connections, connection);
        connection->socket = conn_socket;
        connection->finished = false;
        connection->callback = self->callback;
        pthread_create((pthread_t*) &connection->reader_thread_id, NULL, TCPMSG_reader_thread, connection);

        TCPMSG_remove_finished_threads(self);
    }

    return (void*)0;
}
