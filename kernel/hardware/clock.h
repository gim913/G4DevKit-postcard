#ifndef hw_clock_h
#define hw_clock_h

#include <std/types.h>

enum ClockFlags {
	Auto_Reset      = 0x80000000u,
	Clock_Interrupt = 0x40000000u
};

typedef int (*ClockCallback)(void);

void clockHandleInterrupt(u32 reason, u32 clocks);
void clockSet(u32 id, u32 timeout, ClockCallback callback);
int clockGet(u32* time);

#endif
