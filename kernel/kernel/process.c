#include "process.h"
#include "rawmm.h"
#include "mm.h"
#include <std/memory.h>
#include <std/defines.h>

static u32 g_pid = 1;
static u32 g_tid = 1;
static Process* g_singleProcess;
static Thread* g_singleThread;


void processSubsystemInit() {
	g_singleProcess = NULL;
	g_singleThread = NULL;
}

static bool threadAllocateSlot(Thread* thread) {
	if (g_singleThread) {
		return false;
	}
	
	g_singleThread = thread;
	return true;
}

static bool processAllocateSlot(Process* process) {
	if (g_singleProcess) {
		return false;
	}
	
	g_singleProcess = process;
	return true;
}

CpuContext* threadContinue(Process* process) {
	return &process->pcb.threads[0].ctx;
}

static Process* g_currentProcess;
CpuContext* processContinue() {
	return threadContinue(g_singleProcess);
}

bool threadCreate(Thread* thread, void* startAddress, void* stackBase) {
	memset(thread, 0, sizeof(Thread));
	if (!threadAllocateSlot(thread)) {
		return false;
	}
	
	thread->tid = g_tid++;
	thread->ctx.sp = stackBase;

	thread->entryPoint = startAddress;
	thread->tcb.stackBase = stackBase;
	thread->tcb.state = State_Ready;
}

void asmProcessInit();

/**
 * returns address where to continue execution
 */
void* processInit(Process* process) {
	g_currentProcess = process;
	return process->pcb.threads[0].entryPoint;
}

Process* processCreate(const char* name, void* startAddress, void* stackTop) {
	Process* prc = (Process*)mmAllocate(sizeof(Process));
	if (!prc) {
		return NULL;
	}
	memset(prc, 0, sizeof(Process));
	
	if (!processAllocateSlot(prc)) {
		mmFree(prc);
		return NULL;
	}
	
	// create main thread
	if (!threadCreate(&prc->pcb.threads[0], startAddress, stackTop)) {
		mmFree(prc);
		return NULL;
	}
	
	// set process data
	prc->pid = g_pid++;
	
	// we always start as supervisor, processInit will change that
	prc->pcb.threads[0].ctx.pc = (void*)&asmProcessInit;
	prc->pcb.threads[0].ctx.flags = (1<<26);
	
	// we're gonna pass to processInit current process structure
	prc->pcb.threads[0].ctx.gregs[0] = (u32)prc;	

	return prc;
}