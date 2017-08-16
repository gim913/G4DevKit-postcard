#ifndef hw_interrupts_h
#define hw_interrupts_h

#include <std/types.h>

enum InetrruptBus {
	Bus_Cpu = 0,
	Bus_Clock = 1,
	Bus_Screen = 2,
	Bus_Keyboard = 3,
	Bus_Network_Card = 4,
	Bus_Disk = 5
};

enum InterruptCpu {
	Bus_Cpu_Abort = 0,
	Bus_Cpu_Div = 1,
	Bus_Cpu_Undefined = 2,
	Bus_Cpu_Illegal = 3,
	Bus_Cpu_Swi = 4
};

void interruptsHandle(u32 busAndReason, u32 reg0, u32 reg1, u32 reg2, u32 reg3);

#endif