#include "tcpmsg.h"
#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "config.h"
#include <arpa/inet.h>

/**
 * Reads the given number of bytes. If there are not
 * yet all bytes available it waits until all bytes
 * have been received.
 *
 * @return On success it returns the number of bytes
 *         which had been received. In case of an error
 *         it returns the result of the last call
 *         of function read.
 */
int TCPMSG_read_bytes(int socket, void* buffer, unsigned int n)
{
    int bytes_read = 0; // Contains the sum of all data which had been received
    int result = 1; // Contains the result from function read
    int retval = 0; // Contains the return value of this function

    ENTER_FUNC();

    while (bytes_read < n && result > 0)
    {
        result = read(socket, buffer + bytes_read, n - bytes_read);
        printf("Result from reading socket %d is %d\n", socket, result);
        if (result < 1)
        {
            retval = result;
        }
        else
        {
            bytes_read += result;
            retval = bytes_read;
        }
    }

    LEAVE_FUNC();
    return retval;
}

int TCPMSG_read_message(int socket, uint8_t* buffer)
{
    int result = 0;
    int sync_pattern = 0;
    int payload_length = 0;
    int total_received = 0;
    char string[3 * sizeof(tcpmsg_header_t)];

    ENTER_FUNC();

    memset(string, 0, 3 * sizeof(tcpmsg_header_t));
    
    result = TCPMSG_read_bytes(socket, (void*) &buffer[0],
            sizeof(tcpmsg_header_t));

    if (result <= 0) return result;
    total_received = result;

    tcpmsg_header_t* header_ptr = (tcpmsg_header_t*) buffer;

    bytes2hex((unsigned char*)buffer, sizeof(tcpmsg_header_t), string);
    printf("Received header: %s\n", string);

    sync_pattern = ntohs(header_ptr->sync_pattern);
    if (sync_pattern != SYNC_PATTERN)
    {
        printf("Error in sync pattern\n");
        LEAVE_FUNC();
        return -1;
    }

    payload_length = ntohl(header_ptr->payload_length);
    if (payload_length > (TCPMSG_MAX_MSG_SIZE - sizeof(tcpmsg_header_t)))
    {
        printf("Payload length exceeds maximum size of payload\n");
        LEAVE_FUNC();
        return -1;
    }

    if (payload_length > 0)
    {
        result = TCPMSG_read_bytes(socket,
            (void*) &buffer[sizeof(tcpmsg_header_t)],
            payload_length);

        LEAVE_FUNC();
        if (result <= 0) return result;
        total_received += result;
    }

    LEAVE_FUNC();
    return total_received;
}

void set_tcpmsg_header(
        tcpmsg_header_t* header, int msg_type, int payload_length)
{
    header->sync_pattern = htons(SYNC_PATTERN);
    header->msg_type = htons(msg_type);
    header->payload_length = htonl(payload_length);
}

int get_sync_pattern(tcpmsg_header_t* header)
{
    return ntohs(header->sync_pattern);
}

int get_payload_length(tcpmsg_header_t* header)
{
    return ntohl(header->payload_length);
}

int get_msg_type(tcpmsg_header_t* header)
{
    return htons(header->msg_type);
}

void* TCPMSG_reader_thread(void* ptr)
{
    int result = 1;
    tcpmsg_reader_vars_t* reader_vars = ptr;
    uint8_t tmp_buffer[TCPMSG_MAX_MSG_SIZE];

    printf("Started reader thread with id %ld\n", reader_vars->reader_thread_id);

    memset(tmp_buffer, 0, TCPMSG_MAX_MSG_SIZE);

    while (result > 0)
    {
        result = TCPMSG_read_message(
                reader_vars->socket,
                &tmp_buffer[0]);
        if (result > 0)
        {
            (*reader_vars->callback)(reader_vars, tmp_buffer);
        }
    }
    printf("Return from reader thread with id %ld\n", reader_vars->reader_thread_id);
    reader_vars->finished = true;
    return (void*) 0;
}
