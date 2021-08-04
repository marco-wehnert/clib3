#ifndef __tcpmsg_h__
#define __tcpmsg_h__

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct
{
    uint16_t sync_pattern;
    uint16_t msg_type;
    uint32_t payload_length;
} tcpmsg_header_t;

typedef struct tcpmsg_reader_vars_s
{
    int socket;
    pthread_t reader_thread_id;
    volatile bool finished;
    void (*callback)(struct tcpmsg_reader_vars_s *reader, uint8_t* buffer);
} tcpmsg_reader_vars_t;

int TCPMSG_read_bytes(int socket, void* buffer, unsigned int n);
int TCPMSG_read_message(int socket, uint8_t* buffer);
void* TCPMSG_reader_thread(void* ptr);

void set_tcpmsg_header(
        tcpmsg_header_t* header, int msg_type, int payload_length);
int get_sync_pattern(tcpmsg_header_t* header);
int get_payload_length(tcpmsg_header_t* header);
int get_msg_type(tcpmsg_header_t* header);

#endif
