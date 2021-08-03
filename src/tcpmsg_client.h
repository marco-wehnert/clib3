#ifndef __tcpclient_h__
#define __tcpclient_h__

#include "config.h"

/**
 * Variables for a TCP client
 */
typedef struct
{
    int socket;
    char* server_ip;
    int server_port;
} tcpmsg_client_vars_t;

int  TCPMSG_connect_client(tcpmsg_client_vars_t* vars);
void TCPMSG_disconnect_client(tcpmsg_client_vars_t* vars);

#endif
