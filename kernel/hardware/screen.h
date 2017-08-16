#ifndef hw_screen_h
#define hw_screen_h

#include <std/types.h>

typedef struct
{
	char* buffer;
	int xres;
	int yres;
	int bytesPerCharacter;
} ScreenInfo;

extern ScreenInfo g_screenInfo;
extern int g_currentScreen;

int screenInit(void);
void screenAllocate(u16* addr, int id);
void screenClear(int id);
void screenSet(int id);
void screenSetXy(int x, int y);

void putch(int x, int y, char ch, char color);

void printk(const char* buffer);
void dumpRegisters(u32* regs);
void dumpMemory(u32 addr, u8* d, int dataSize);

#endif
