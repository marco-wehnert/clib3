#ifndef __utils_h__
#define __utils_h__

#include <errno.h>
#include <stdlib.h> // needed for function exit(...);

// #define handle_error(en) \ //
//    do { errno = en; perror(NULL); exit(EXIT_FAILURE); } while (0)

void bytes2hex(unsigned char* bytes, int size, char* string);

#define ENTER_FUNC() printf(">>> %*s\n", -40, __func__)
#define LEAVE_FUNC() printf("<<< %*s\n", -40, __func__)

#endif
