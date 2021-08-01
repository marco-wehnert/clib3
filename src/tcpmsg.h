#ifndef __tcpmsg_h__
#define __tcpmsg_h__

#include <stdint.h>

typedef struct
{
    uint16_t sync_pattern;
    uint16_t msg_type;
    uint32_t payload_length;
} tcpmsg_header_t;


typedef struct
{
    int socket;

} tcpmsg_receiver_vars_t;


void TCPMSG_read_bytes(int socket, void* buffer, unsigned int n);

#endif
