#ifndef kernel_mm_h
#define kernel_mm_h

#include <std/types.h>

int mmSubsystemInit(u8* p, u32 size);

void* mmAllocate(u32 size);
void mmFree(void* memPtr);

#endif