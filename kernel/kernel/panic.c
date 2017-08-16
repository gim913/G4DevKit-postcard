#include "panic.h"
#include "hardware/cpu.h"
#include "hardware/screen.h"

static int panicState;

void kernelPanicSet(int state) {
	panicState = state;
}

void kernelPanic(const char* msg) {
	if (panicState) {
		screenClear(0);
		screenSet(0);
		printk(msg);
	}
	while (1) {
		cpuHalt();
	}
}