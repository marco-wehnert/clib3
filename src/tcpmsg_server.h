#ifndef __tcpmsg_server_h__
#define __tcpmsg_server_h__

#include <pthread.h>

typedef struct
{
    int socket;
    int port;
    pthread_t listen_thread;
} tcpmsg_server_vars_t;

typedef struct
{
    int socket;
} tcpmsg_connection_vars_t;

int TCPMSG_create_server(tcpmsg_server_vars_t* vars);
void TCPMSG_shutdown_server(tcpmsg_server_vars_t* vars);
void* TCPMSG_listen_thread(void*);

#endif
