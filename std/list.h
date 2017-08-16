#ifndef std_list_h
#define std_list_h

#include "types.h"

/* list implementation operates on predefined chunk of data,
 * list implementation is *funny*, it uses indexes instead of pointers...
 * (to save space on every list entry), but that makes things bit complicated
 * and probably slow, so I might change it into normal implementation at some point
 */

typedef u16 Idx;
enum { List_Stop_Marker = 0xffffu };

typedef struct List {
	u8* listStart;
	u32 memSize;
	u32 elemSize;
	Idx currentSize;
	Idx firstItem;
	Idx lastItem;
} List;

struct ListItem;
typedef struct ListItem ListItem;

ListItem* listIterStart(List* l);
ListItem* listIterNext(List* l, ListItem* i);
void* listIterData(ListItem* i);

bool listInit(List* l, u32 elemSize, u8* ptr, u32 size);
ListItem* listRemove(List* l, ListItem* toRemove);

ListItem* listInsertAfter(List* l, ListItem* i, void* elem);
ListItem* listAppend(List* l, void* elem);

#endif