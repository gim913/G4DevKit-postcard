#include "cpu.h"
#include "hwicall.h"

enum CpuInt {
	Get_Ram = 0,
	Set_Mmu = 2
};

u32 cpuGetRam(void) 
{
	return hwiCall_0_1(INT(Bus_Cpu,Get_Ram));
}

u32 cpuMmuSet(void* ptr)
{
	return hwiCall_1_0(INT(Bus_Cpu, Set_Mmu), (u32)ptr);
}