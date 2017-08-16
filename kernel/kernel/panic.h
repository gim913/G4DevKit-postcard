#ifndef kernel_panic_h
#define kernel_panic_h

void kernelPanic(const char* msg);

enum { State_Boot = 0, State_Initialized = 1 };
void kernelPanicSet(int state);

#endif