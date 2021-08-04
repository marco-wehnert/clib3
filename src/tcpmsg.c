#include "tcpmsg.h"
#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "config.h"

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

//    printf(">>> void TCPMSG_read_bytes(int socket, void* buffer, unsigned int n)\n");
    while (bytes_read < n && result > 0)
    {
        result = read(socket, buffer + bytes_read, n - bytes_read);
        printf("    TCPMSG function call read(...) did return %d, socket = %d\n", result, socket);
        if (result < 1)
        {
//            printf("TCPMSG Leave function because read(...) "
//                    "did return a value < 1\n");
            retval = result;
        }
        else
        {
            bytes_read += result;
            retval = bytes_read;
        }
    }

//    printf("<<< void TCPMSG_read_bytes(int socket, void* buffer, unsigned int n)\n");
    return retval;
}

int TCPMSG_read_message(int socket, uint8_t* buffer)
{
    int result;
    char string[3 * sizeof(tcpmsg_header_t)];

    memset(string, 0, 3 * sizeof(tcpmsg_header_t));
    result = TCPMSG_read_bytes(socket, (void*) &buffer[0],
            sizeof(tcpmsg_header_t));

    if (result <= 0) return result;

    tcpmsg_header_t* header_ptr = (tcpmsg_header_t*) buffer;
    bytes2hex((unsigned char*)buffer, sizeof(tcpmsg_header_t), string);
    printf("Received header: %s\n", string);

    result = TCPMSG_read_bytes(socket,
            (void*) &buffer[sizeof(tcpmsg_header_t)],
            header_ptr->payload_length);

    if (result <= 0) return result;
    return result + sizeof(tcpmsg_header_t);
}

void* TCPMSG_reader_thread(void* ptr)
{
    int result = 1;
    tcpmsg_reader_vars_t* thread_vars_ptr = ptr;
    uint8_t tmp_buffer[TCPMSG_MAX_MSG_SIZE];

//    printf(">>> void* TCPMSG_receiver_thread(void* ptr)\n");

//    printf("TCPMSG pointer passed to thread 'TCPMSG_reader_thread' is %p\n", ptr);
    memset(tmp_buffer, 0, TCPMSG_MAX_MSG_SIZE);

    while (result > 0)
    {
        result = TCPMSG_read_message(thread_vars_ptr->socket, &tmp_buffer[0]);
    }
//    printf("<<< void* TCPMSG_receiver_thread(void* ptr)\n");
    return (void*) 0;
}
