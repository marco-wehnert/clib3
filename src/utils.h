#ifndef __utils_h__
#define __utils_h__

#include <errno.h>

#define handle_error(en) \
    do { errno = en; perror(NULL); exit(EXIT_FAILURE); } while (0)

#endif
