#ifndef __tcpmsg_server_h__
#define __tcpmsg_server_h__

#include "linked_list.h"
#include <pthread.h>

typedef struct
{
    int socket;
    int port;
    linked_list_t connections;
    pthread_t listen_thread_id;
    void (*callback)(struct tcpmsg_reader_vars_s *reader, uint8_t* buffer);
} tcpmsg_server_vars_t;


int TCPMSG_create_server(tcpmsg_server_vars_t* vars);
void TCPMSG_close_all_server_connections(tcpmsg_server_vars_t* self);
void TCPMSG_shutdown_server(tcpmsg_server_vars_t* vars);
void* TCPMSG_listen_thread(void*);

#endif
