#ifndef std_memory_h
#define std_memory_h

#include "types.h"

void memcpy(void* dest, const void* src, size_t count);
void memset(void* dest, char c, size_t count);

#endif