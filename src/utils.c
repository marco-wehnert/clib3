#include "utils.h"
#include <stdio.h>


char* charset = "0123456789ABCDEF";

void bytes2hex(unsigned char* bytes, int size, char* string)
{
    int i;
    unsigned char c;
    for (i = 0; i < size; i++)
    {
        c = bytes[i];
        string[i * 3]     =  charset[c >> 4];
        string[i * 3 + 1] = charset[c & 0xF];
        string[i * 3 + 2] = ' ';
    }
    string[(i-1) * 3 + 2] = 0;
}
