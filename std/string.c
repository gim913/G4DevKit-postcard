#include "string.h"

size_t strlen(const char* str) {
	size_t l = 0;
	while (*str++) l++;
	return l;
}
