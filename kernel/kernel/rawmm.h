#ifndef kernel_rawmm_h
#define kernel_rawmm_h

#include <std/types.h>

/* Initializes raw memory manager susbsystem.
 * Requires amount of ram as a \a rawSize
 */
void rawmmSubsystemInit(u32 ramSize);

/* Allocate \a size bytes (starting from the top of the memory).
 * Allocations are always rounded up to page size.
 * Returns pointer to beginning of a region.
 */
u8* rawmmAllocate(u32 size);

#endif