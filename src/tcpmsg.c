#include "tcpmsg.h"
#include <stdio.h>
#include <unistd.h>
#include "config.h"

/**
 * Reads the given number of bytes
 */
void TCPMSG_read_bytes(int socket, void* buffer, unsigned int n)
{
    int bytes_read = 0;
    int result = 0;

    while (bytes_read < n)
    {
        result = read(socket, buffer + bytes_read, n - bytes_read);
        if (result < 1)
        {
            fprintf(stderr, "Read bytes from socket failed");
            return;
        }
        bytes_read += result;
    }
}

void* TCPMSG_receiver_thread(void* ptr)
{
    tcpmsg_receiver_vars_t* thread_vars_ptr = ptr;

    uint8_t tmp_buffer[TCPMSG_MAX_MSG_SIZE];
    while (1)
    {
        TCPMSG_read_bytes(thread_vars_ptr->socket,
                (void*) &tmp_buffer[0],
                sizeof(tcpmsg_header_t));

        tcpmsg_header_t* header_ptr = (tcpmsg_header_t*) tmp_buffer;

        TCPMSG_read_bytes(thread_vars_ptr->socket,
                (void*) &tmp_buffer[sizeof(tcpmsg_header_t)],
                header_ptr->payload_length);

    }
}
