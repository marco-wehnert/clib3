#ifndef __tcpclient_h__
#define __tcpclient_h__

#include "tcpmsg.h"
#include "config.h"

int  TCPMSG_connect_client(tcpmsg_reader_vars_t* vars, char* server_ip, int port);
void TCPMSG_disconnect_client(tcpmsg_reader_vars_t* vars);

#endif
