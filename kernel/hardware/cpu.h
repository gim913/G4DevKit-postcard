#ifndef hw_cpu_h
#define hw_cpu_h

#include <std/types.h>

u32 cpuGetRam(void);
u32 cpuMmuSet(void*);
void cpuDivByZero(void);
void cpuHalt(void);

typedef struct {
	u32 gregs[12];
	void* ip;
	void* sp;
	void* lr;
	void* pc;
	u32 flags;
	double fregs[16];
} CpuContext;


#endif