#include "clock.h"
#include "hwicall.h"
#include "kernel/panic.h"

enum ClockInt {
	Read_Boottime = 0,
	Read_Timer = 1,
	Set_Timer = 2
};

enum {
	Reason_Timer_Reached_Zero = 0
};

/* there are only 8 timers allowed by the ApCpu */
ClockCallback callbacks[8];

void clockHandleInterrupt(u32 reason, u32 clocks) {
	if (reason != Reason_Timer_Reached_Zero) {
		kernelPanic("unknown reason of clock interrupt");
	}
	
	if (clocks & 0x0f) {
		if (clocks & 0x03) {
			if (clocks & 0x01) { (callbacks[0])(); }
			else { /*    0x02 */ (callbacks[1])();}
		} else {
			if (clocks & 0x04) { (callbacks[2])(); }
			else { /*    0x08 */ (callbacks[3])();}
		}
	} else {
		if (clocks & 0x30) {
			if (clocks & 0x10) { (callbacks[4])(); }
			else { /*    0x20 */ (callbacks[5])();}
		} else {
			if (clocks & 0x40) { (callbacks[6])(); }
			else { /*    0x80 */ (callbacks[7])();}
		}
	}
}

void clockSet(u32 id, u32 timeout, ClockCallback cb) {
	hwiCall_2_0(INT(Bus_Clock, Set_Timer), id, timeout);
	id &= 0xff;
	if (id >= 0 && id < 8) {
		callbacks[id] = cb;
	}
}

int clockGet(u32* ret) {
	return hwiCall_0_2(INT(Bus_Clock, Read_Boottime), ret);
}