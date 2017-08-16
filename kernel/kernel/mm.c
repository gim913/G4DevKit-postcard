#include "mm.h"
#include "defines.h"
#include <std/defines.h>
#include <std/memory.h>
#include <std/list.h>

//#include <stdio.h>
#define printf

// buckets sizes, must be multiple of 8
#define B1_CNT 256
#define B2_CNT 128
#define B3_CNT 64

#define B1_SIZE 16
#define B2_SIZE 256
#define B3_SIZE 1024

#define SIZE_IN_BITS 1

// buckets
static u8* b0016;
static u8* b0256;
static u8* b1024;

// regions
static u8* m0016;
static u8* m0256;
static u8* m1024;

#define BUCKET_SIZE(cnt) (((cnt)*SIZE_IN_BITS + 7) / 8)

u8* freeRangeStart;
static u8* freeRangeEnd;

typedef struct _FreeRangeElem {
	u32 val;
} FreeRangeElem;

List freeRangeList;

int mmSubsystemInit(u8* p, u32 size) {
	if (size < BUCKET_SIZE(B1_CNT) + BUCKET_SIZE(B2_CNT) + BUCKET_SIZE(B3_CNT)
		+ B1_SIZE * B1_CNT
		+ B2_SIZE * B2_CNT
		+ B3_SIZE * B3_CNT) {
		return -1;
	}
	if (b0016) {
		// memory manager subsystem already initialized
		return -2;
	}

	b0016 = p;
	b0256 = b0016 + BUCKET_SIZE(B1_CNT);
	b1024 = b0256 + BUCKET_SIZE(B2_CNT);

	memset(b0016, 0, BUCKET_SIZE(B1_CNT));
	memset(b0256, 0, BUCKET_SIZE(B2_CNT));
	memset(b1024, 0, BUCKET_SIZE(B3_CNT));

	m0016 = b1024 + BUCKET_SIZE(B3_CNT);;
	m0256 = m0016 + B1_SIZE * B1_CNT;
	m1024 = m0256 + B2_SIZE * B2_CNT;

	u8* temp = m1024 + B3_SIZE * B3_CNT;
	listInit(&freeRangeList, sizeof(FreeRangeElem), temp, 16 * PAGE_SIZE);

	freeRangeStart = temp + 16*PAGE_SIZE;
	freeRangeEnd = p + size;

	printf("%p\n", b0016);
	printf("%p\n", b0256);
	printf("%p\n", b1024);
	printf("\n");
	printf("  16: %p\n", m0016);
	printf(" 256: %p\n", m0256);
	printf("1024: %p\n", m1024);
	printf("%p\n", freeRangeStart);

	printf("region : %d\n", freeRangeEnd - freeRangeStart);
	printf("%d\n", BUCKET_SIZE(B1_CNT) + BUCKET_SIZE(B2_CNT) + BUCKET_SIZE(B3_CNT)
		+ B1_SIZE * B1_CNT
		+ B2_SIZE * B2_CNT
		+ B3_SIZE * B3_CNT);

	return 0;
}

static void* allocBucket(u8* bucket, int bucketCount, u8* region, int memSize) {
	int i = 0;
	for (; i < bucketCount / 8; ++i) {
		if (bucket[i] != 0xff) {
			break;
		}
	}
	int b = 1;
	int j = 0;
	for (; j < 8; ++j) {
		if ((bucket[i] & b) == 0) {
			break;
		}
		b <<= 1;
		printf("BIT: %d\n", b);
	}
	printf("bucket %d,%d is not fully occupied\n", i, j);
	u8* r = region + (i * 8 + j) * memSize;
	bucket[i] |= b;
	return r;
}


static void compactFreeRanges() {
	
}

enum
{
	Flag_Occupied = 0x01000000u,
	Size_Mask = 0xffffffu,
	Flag_Mask = 0xff000000u
};

void* findSpot(u32 size) {
	u8* result = freeRangeStart;

	// skip 'used'
	for (ListItem* i = listIterStart(&freeRangeList); i; i = listIterNext(&freeRangeList, i)) {
		FreeRangeElem* current = (FreeRangeElem*)listIterData(i);
		u32 val = current->val;
		u32 blockSize = val & Size_Mask;
		u32 flags = val & Flag_Mask;
		if (0 == (flags & Flag_Occupied) && size <= blockSize) {
			if (blockSize < size) {
				// store 'rest' as free block
				FreeRangeElem next = { (blockSize - size) };
				listInsertAfter(&freeRangeList, i, &next);
				// modify size of current block
				current->val = size;
			}

			current->val |= Flag_Occupied;
			return result;
		}

		result += blockSize;
	}

	if (result + size < freeRangeEnd) {
		FreeRangeElem entry = { size | Flag_Occupied };
		listAppend(&freeRangeList, &entry);

		return result;
	} else {
		return NULL;
	}
}

void* allocateFreeRange(u32 size) {
	compactFreeRanges();

	return findSpot(size);
}

void freeFreeRange(void* ptr) {
	u8* result = freeRangeStart;

	// skip 'used'
	for (ListItem* i = listIterStart(&freeRangeList); i; i = listIterNext(&freeRangeList, i)) {
		FreeRangeElem* current = (FreeRangeElem*)listIterData(i);
		if (result == ptr) {
			current->val &= Size_Mask;
			printf("freed\n");
			break;
		}
		
		u32 val = current->val;
		u32 blockSize = val & Size_Mask;
		result += blockSize;
	}
}

void* mmAllocate(size_t s) {
	void* buck = 0;
	if (s <= B1_SIZE) {
		buck = allocBucket(b0016, B1_CNT, m0016, B1_SIZE);
	} else if (s <= B2_SIZE) {
		buck = allocBucket(b0256, B2_CNT, m0256, B2_SIZE);
	} else if (s <= B3_SIZE) {
		buck = allocBucket(b1024, B3_CNT, m1024, B3_SIZE);
	}

	if (!buck) {
		buck = allocateFreeRange(s);
	}

	return buck;
}

static int freeBucket(u8* bucket, int bucketIndex) {
	int bucketId = bucketIndex / 8;
	int bucketBit = bucketIndex & 7;

	int bit = 1 << bucketBit;

	int wasAllocated = bucket[bucketId] & bit;
	bucket[bucketId] &= (0xff ^ bit);
	return wasAllocated;
}

void mmFree(void* memPtr) {
	u8* p = (u8*)memPtr;
	if (p >= m0016 && p < m0016 + B1_SIZE * B1_CNT) {
		int id = (p - m0016) / B1_SIZE;
		freeBucket(b0016, id);
		printf("free: b16,%d\n", id);

	} else if (p >= m0256 && p < m0256 + B2_SIZE * B2_CNT) {
		int id = (p - m0256) / B2_SIZE;
		freeBucket(b0256, id);
		printf("free: b256,%d\n", id);

	} else if (p >= m1024 && p < m1024 + B3_SIZE * B3_CNT) {
		int id = (p - m1024) / B3_SIZE;
		freeBucket(b1024, id);
		printf("free: b1024,%d\n", id);
	} else {
		freeFreeRange(memPtr);
	}
}
