#include "screen.h"
#include "hwicall.h"

#include <std/format.h>
#include <std/memory.h>
#include <std/string.h>

ScreenInfo g_screenInfo;

static u16* g_screens[2];

enum ScreenInt {
	Get_Info = 0,
	Map_Buffer = 1
};

void screenGetInfo(ScreenInfo* scr)
{
	hwiCall_0_4(INT(Bus_Screen,Get_Info), (u32*)&g_screenInfo);
}

void screenMap(void* screenPtr) 
{
	hwiCall_1_0(INT(Bus_Screen,Map_Buffer), (u32)screenPtr);
}

int g_currentScreen = 0;
static int printkScreen = 0;
static int curx;
static int cury;

int screenInit(void)
{
	printkScreen = 0;
	screenGetInfo(&g_screenInfo);
	return g_screenInfo.bytesPerCharacter * g_screenInfo.xres * g_screenInfo.yres;
}

void screenAllocate(u16* addr, int id) {
	g_screens[id] = addr;
}

bool validId(int id) {
	return (id == 0 || id == 1);
}

void screenClear(int id) {
	if (!validId(id))
		return;

	memset(g_screens[id], 0, g_screenInfo.bytesPerCharacter * g_screenInfo.xres * g_screenInfo.yres);
}

void screenSet(int id) {
	if (id != 0 && id != 1)
		id = 0;
	screenMap(g_screens[id]);

	curx = 0;
	cury = 0;
	g_currentScreen = id;
}

void screenSetXy(int x, int y) {
	curx = x;
	cury = y;
}

void putch(int x, int y, char ch, char color) {
	u16* ptr = g_screens[1 - g_currentScreen];
	if (!ptr)
		return;
	
	ptr += y * g_screenInfo.xres;
	ptr += x;
	
	u16 value = color;
	*ptr++ = (value << 8) | ch;
}

void printk(const char* buffer) {
	u16* ptr = g_screens[1 - g_currentScreen];
	if (!ptr)
		return;
		
	ptr += cury * g_screenInfo.xres;
	ptr += curx;
	
	const char* p = buffer;
	while (*p)
		*ptr++ = 0xf00 | *p++;
}

void dumpRegisters(u32* regs) {
	Format _f;
	Format* f = &_f;
	formatInit(f);
	
	for (int i = 0; i < 16; ++i) {
		char* text = formatFormat(f, "R{:d2} {:x8} = {,9:d}", i, regs[i], regs[i]);
		cury = i + 1;
		printk(text);
	}
}

void dumpMemory(u32 addr, u8* d, int dataSize) {
	Format _f;
	Format* f = &_f;
	formatInit(f);

	for (int i = 0; i < dataSize; i += 16) {
		char* text = formatFormat(f,
			"{:x8}: = {:x2}{:x2} {:x2}{:x2} {:x2}{:x2} {:x2}{:x2}  {:x2}{:x2} {:x2}{:x2} {:x2}{:x2} {:x2}{:x2}",
			addr, d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8], d[9], d[10], d[11], d[12], d[13], d[14], d[15]);
		
		cury = i/16 + 1;
		curx = 27;
		printk(text);
		addr += 16;
		d += 16;
	}
}
