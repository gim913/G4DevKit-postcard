.text

public _cpuDivByZero
_cpuDivByZero:
	mov r0, 0
	sdiv r0, r0, r0
	
public _cpuHalt
_cpuHalt:
	hlt
	mov pc, lr