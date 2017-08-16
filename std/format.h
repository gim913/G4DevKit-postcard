#ifndef std_format_h
#define std_format_h

#include "types.h"

#define Buf_Size 1024

typedef struct {
	char dataBuf[Buf_Size];
	size_t pos; // pos inside the dataBuf
	bool indexPresent;
	int index;
	bool alignmentPresent;
	bool alignmentSign;
	size_t alignment;

	const char* currentFormat;
	size_t currentFormatSize;
	bool needsProcessing;

	const char* p; // pointer to currently analyzed piece of format string
	const char* last; // pointer to last "finished" format part
	const char* end; // pointer to end of format string
} Format;

void formatInit(Format* f);
char* formatFormat(Format* f, const char* msg, ...);

#endif
