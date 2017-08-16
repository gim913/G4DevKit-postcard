#ifndef hw_hwicall_h
#define hw_hwicall_h

#include "interrupts.h"
#include <std/types.h>


#define INT(a,b) (((a)<<24)|(b))

/* all hwi calls share format : _number-of-arguments _ number-of-returned */

int hwiCall_0_1(int hwiNum);
int hwiCall_0_2(int hwiNum, u32* ret);
int hwiCall_0_4(int hwiNum, u32* ret);

int hwiCall_1_0(int hwiNum, u32 arg1);
int hwiCall_2_0(int hwiNum, u32 arg1, u32 arg2);
int hwiCall_3_0(int hwiNum, u32 arg1, u32 arg2, u32 arg3);

#endif
