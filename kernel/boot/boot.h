#ifndef _BOOT_H_
#define _BOOT_H_

/*! Markers to check the size of the interrupt context.
 */
extern const int mainCpuCtx;
extern const int mainCpuCtxEnd;

/*!
 *	Filled by linker on ROM preparing stage.
 */
typedef struct ROMProcessInfo
{
	unsigned int readOnlyAddr; // address
	unsigned int readOnlySize; // size in bytes
	unsigned int readWriteAddr; // address
	unsigned int readWriteSize; // size in bytes
	unsigned int sharedReadWriteAddr;
	unsigned int sharedReadWriteSize;
} ROMProcessInfo;
extern ROMProcessInfo processInfo;

#define NO_INTERRUPT -1
extern const int kernelCurrIntrBusAndReason;
extern const int kernelPrevIntrBusAndReason;
#endif