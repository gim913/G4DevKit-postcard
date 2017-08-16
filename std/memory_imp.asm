.text

public _memcpy
_memcpy:
	memcpy [r0], [r1], r2
	mov pc, lr

public _memset
_memset:
	memset [r0], r1, r2
	mov pc, lr