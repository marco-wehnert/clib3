#ifndef __tcpmsg_h__
#define __tcpmsg_h__

#include <stdint.h>
#include <pthread.h>

typedef struct
{
    uint16_t sync_pattern;
    uint16_t msg_type;
    uint32_t payload_length;
} tcpmsg_header_t;

typedef struct
{
    int socket;
    pthread_t reader_thread_id;
} tcpmsg_reader_vars_t;

int TCPMSG_read_bytes(int socket, void* buffer, unsigned int n);
void* TCPMSG_reader_thread(void* ptr);

#endif
