#include "rawmm.h"
#include "defines.h"

static u8* g_memoryTop;

void rawmmSubsystemInit(u32 ramSize) {
	g_memoryTop = (u8*)ramSize;
}

// we want all memory allocated via raw MM to be
// aligned to page boundary

u8* rawmmAllocate(u32 size) {
	size = ALIGN_PAGE_UP(size);
	g_memoryTop -= size;
	return g_memoryTop;
}