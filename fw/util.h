#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdint.h>

#define SINE_LUT_SIZE 256

extern const uint16_t sine_lut[SINE_LUT_SIZE];

void *memset(void *dst, int value, size_t n);
void memcpy(void *dst, void *src, size_t n);

#endif /* UTIL_H */

