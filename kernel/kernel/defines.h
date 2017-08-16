#ifndef kernel_defines_h
#define kernel_defines_h

#define PAGE_SIZE 1024

#define ALIGN_PAGE_UP(x) (((x) + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1))

#endif