#include "list.h"
#include "defines.h"
#include "memory.h"

// needed, as apparently compiler makes data[1] from data[0] above
const u32 List_Item_Size = 8;

typedef struct ListItem
{
	Idx prev;
	Idx next;
	u8 data[0];
} ListItem;

ListItem* listIterStart(List* l) {
	if (0 == l->currentSize) {
		return NULL;
	}

	const u32 Item_Size = l->elemSize + List_Item_Size;
	return (ListItem*)(l->listStart + l->firstItem * Item_Size);
}

ListItem* listIterNext(List* l, ListItem* i) {
	if (i->next == List_Stop_Marker) {
		return NULL;
	}

	const u32 Item_Size = l->elemSize + List_Item_Size;
	ListItem* p = (ListItem*)(l->listStart + i->next * Item_Size);
	return p;
}

void* listIterData(ListItem* i) {
	return i->data;
}

bool listInit(List* l, u32 elemSize, u8* ptr, u32 size) {
	const u32 Item_Size = elemSize + List_Item_Size;
	if (elemSize + Item_Size > size) {
		return false;
	}
	l->elemSize = elemSize;
	l->listStart = ptr;
	l->memSize = size;
	l->currentSize = 0;
	l->firstItem = List_Stop_Marker;
	l->lastItem = List_Stop_Marker;
	return true;
}

ListItem* listRemove(List* l, ListItem* toRemove) {
	const u32 Item_Size = l->elemSize + List_Item_Size;
	Idx prev = toRemove->prev;
	Idx next = toRemove->next;
	Idx toRemoveIdx = List_Stop_Marker;

	if (l->currentSize == 0) {
		return NULL;
	}
	
	// fix pointers
	if (List_Stop_Marker != prev) {
		ListItem* p = (ListItem*)(l->listStart + prev * Item_Size);
		toRemoveIdx = p->next;
		p->next = next;
	}

	ListItem* result = NULL;
	if (List_Stop_Marker != next) {
		result = (ListItem*)(l->listStart + next * Item_Size);
		toRemoveIdx = result->prev;
		result->prev = prev;
	}

	if (l->currentSize == 1) {
		toRemoveIdx = 0;
	}

	// fix indexes
	if (l->lastItem == toRemoveIdx) {
		l->lastItem = prev;
	}
	if (l->firstItem == toRemoveIdx) {
		l->firstItem = next;
	}

	// move element at the end into free space
	u32 oldLocation = l->currentSize - 1;
	u32 newLocation = toRemoveIdx;
	ListItem* endItem = (ListItem*)(l->listStart +  oldLocation * Item_Size);
	if (l->currentSize > 1 && toRemove != endItem) {
		memcpy(toRemove, endItem, Item_Size);

		// now need to fix pointers for endItems prev and next
		Idx prev = endItem->prev;
		Idx next = endItem->next;

		if (List_Stop_Marker != prev) {
			((ListItem*)(l->listStart + prev * Item_Size))->next = newLocation;
		}

		if (List_Stop_Marker != next) {
			((ListItem*)(l->listStart + next * Item_Size))->prev = newLocation;
		}

		if (l->lastItem == oldLocation) {
			l->lastItem = newLocation;
		}

		if (l->firstItem == oldLocation) {
			l->firstItem = newLocation;
		}
	}

	--l->currentSize;
	return result;
}

ListItem* listAppend(List* l, void* elem) {
	const u32 Item_Size = l->elemSize + List_Item_Size;
	ListItem* newNode = (ListItem*)(l->listStart + l->currentSize * Item_Size);
	
	if (l->currentSize == List_Stop_Marker) {
		return NULL;
	}
	if ((u8*)newNode + Item_Size > l->listStart + l->memSize) {
		return NULL;
	}

	newNode->next = List_Stop_Marker;

	if (List_Stop_Marker != l->lastItem) {
		newNode->prev = l->lastItem;
		ListItem* prev = (ListItem*)(l->listStart + l->lastItem * Item_Size);
		prev->next = l->currentSize;

	} else {
		newNode->prev = List_Stop_Marker;
	}
	
	memcpy(newNode->data, elem, l->elemSize);
	l->lastItem = l->currentSize;
	if (0 == l->currentSize) {
		l->firstItem = 0;
	}
	l->currentSize++;
	return newNode;
}

ListItem* listInsertAfter(List* l, ListItem* i, void* elem) {
	const u32 Item_Size = l->elemSize + List_Item_Size;
	ListItem* newNode = (ListItem*)(l->listStart + l->currentSize * Item_Size);

	if (List_Stop_Marker == l->currentSize || NULL == i) {
		return NULL;
	}
	if ((u8*)newNode + Item_Size > l->listStart + l->memSize) {
		return NULL;
	}

	// fix pointers
	Idx iIdx = List_Stop_Marker;
	if (List_Stop_Marker != i->next) {
		ListItem* next = (ListItem*)(l->listStart + i->next * Item_Size);
		iIdx = next->prev;
		next->prev = l->currentSize;
		newNode->next = i->next;
	} else {
		newNode->next = List_Stop_Marker;
		iIdx = l->lastItem;
	}

	newNode->prev = iIdx;
	i->next = l->currentSize;

	// set elem and list data
	memcpy(newNode->data, elem, l->elemSize);
	// "i" was last, so now, we're last
	if (l->lastItem == iIdx) {
		l->lastItem = l->currentSize;
	}
	l->currentSize++;
	return newNode;
}