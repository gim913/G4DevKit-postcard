#include "interrupts.h"
#include "clock.h"
#include "screen.h"
#include "kernel/process.h"
#include "boot/boot.h"
#include <std/format.h>

extern CpuContext* kernelInterruptedContext;

void interruptsHandle(u32 busAndReason, u32 reg0, u32 reg1, u32 reg2, u32 reg3)
{
	u8 bus = busAndReason >> 24;
	u32 reason = busAndReason & 0x80FFFFFF;
	
	switch (bus) {
		case Bus_Cpu:
			break;
		case Bus_Clock:
			clockHandleInterrupt(reason, reg0);
			return;
		case Bus_Screen:
			break;
		case Bus_Keyboard:
			break;
		case Bus_Network_Card:
			break;
		case Bus_Disk:
			break;
	}
	
	Format _f;
	Format* f = &_f;
	formatInit(f);
	char* text = formatFormat(f, "INTERRUPT p:{:X8} c:{:X8} || {:x8} {:x8} | {:x8}",
		kernelPrevIntrBusAndReason,
		kernelCurrIntrBusAndReason,
		bus,
		reason, reg0);
	
	screenSetXy(0, 0);
	screenSet(0);
	printk(text);
}