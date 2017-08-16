#ifndef kernel_process_h
#define kernel_process_h

#include "hardware/cpu.h"

// for now only constant 16 threads

struct Process;

enum ThreadState {
	State_Init = 0,
	State_Ready = 1,
	State_Running = 2
};

typedef struct {
	void* stackBase;
	int state;
} ThreadObject;

typedef struct {
	ThreadObject tcb;
	u32 tid;
	void* entryPoint;
	CpuContext ctx;
} Thread;

typedef struct {
	u32 kernelTime;
	Thread threads[1];
} ProcessObject;

typedef struct {
	ProcessObject pcb;
	char name[32];
	u32 pid;
} Process;

bool threadCreate(Thread* thread, void* startAddress, void* stackTop);

void processSubsystemInit();
Process* processCreate(const char* name, void* startAddress, void* stackTop);
CpuContext* processContinue();

#endif