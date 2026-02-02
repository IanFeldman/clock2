#include "util.h"

void *memset(void *dst, int value, size_t n)
{
    unsigned char *p = dst;
    while (n--)
    {
        *p++ = (unsigned char)value;
    }
    return dst;
}


void memcpy(void *dst, void *src, size_t n)
{
    // typecast src and dst addresses to (char *)
    char *csrc = (char *)src;
    char *cdst = (char *)dst;

    // copy contents of src[] to dst[]
    for (int i = 0; i < n; i++)
    {
        cdst[i] = csrc[i];
    }
}

