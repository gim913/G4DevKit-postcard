#include "kernel.h"
#include "defines.h"
#include "mm.h"
#include "panic.h"
#include "postcard.h"
#include "process.h"
#include "rawmm.h"

#include "boot/boot.h"
#include "hardware/clock.h"
#include "hardware/cpu.h"
#include "hardware/screen.h"
#include "hardware/interrupts.h"
#include <std/defines.h>
#include <std/format.h>
#include <std/memory.h>

static CpuContext* g_rootContext;
static CpuContext* g_interruptContext;
static u8* g_interruptHandlerStack;

void* kernelHandleInterrupt(u32 r0, u32 r1, u32 r2, u32 r3) {
	u32 busAndReason = kernelCurrIntrBusAndReason;
	
	interruptsHandle(busAndReason, r0, r1, r2, r3);
	
	return processContinue();
}

int handleMainTimer();

void kernelIdleProcess();

/**
 * Initialize subsystems
 * return pointer to a context of a main kernel process
 */ 
void* kernelInit() {
	u32 ram = cpuGetRam();
	rawmmSubsystemInit(ram);
	
	u32 size = screenInit();
	screenAllocate((u16*)rawmmAllocate(size), 0);
	screenAllocate((u16*)rawmmAllocate(size), 1);
	
	u32 mmuSize = ram / (PAGE_SIZE / 4);
	u8* mmu = rawmmAllocate(mmuSize);
	memset(mmu, 0, mmuSize);
	cpuMmuSet(mmu);
	
	kernelPanicSet(State_Boot);
	
	/* get the top pointer and allocate for interrupt handler stack */
	g_interruptHandlerStack = rawmmAllocate(0);
	rawmmAllocate(10*1024);
	void* guardPage = rawmmAllocate(1024);
	
	void* idleProcessStack = rawmmAllocate(0);
	rawmmAllocate(10*1024);
	void* guardPage2 = rawmmAllocate(1024);
	
	// kernel allocator
	// XXX: this might fail miserably :P
	u32 mmSize = 150 * 1024;
	if (0 != mmSubsystemInit(rawmmAllocate(mmSize), mmSize)) {
		kernelPanic("couldn't initialize mm subsystem");
	}
	
	processSubsystemInit();

	Process* main = processCreate("System idle", (void*)&kernelIdleProcess, idleProcessStack);
	g_interruptContext = (CpuContext*)8;
	g_rootContext = &main->pcb.threads[0].ctx;
	
	return (void*)g_rootContext;
}


/* at this point interrupts are active */
void kernelIdleProcess() {
	memset(g_interruptContext, 0, sizeof(CpuContext));
	g_interruptContext->sp = g_interruptHandlerStack;
	
	screenClear(0);
	screenSet(0);
	postcardCtor();
	
	kernelPanicSet(State_Initialized);
	
	// could use the clock, but it doesn't make sense to slow it down.
	//clockSet(0 | Clock_Interrupt | Auto_Reset, 10, &handleMainTimer);
	while(1) {
		handleMainTimer();
	}
}

int handleMainTimer() {
	u32 time[2];
	int ret;

	Format _f;
	Format* f = &_f;
	
	formatInit(f);
	ret = clockGet(time);
	char* text = formatFormat(f, "kernel time handler: {:X8} {:X8} {:X8} root ctx: {:x8}", ret, time[0], time[1], g_rootContext);
	
	postcardUpdate();
	screenClear(1 - g_currentScreen);
	postcardDraw();
	
	screenSetXy(0, 24);
	printk(text);
	screenSet(1 - g_currentScreen);
	return 0;
}
