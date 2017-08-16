.text

extern _processInit

; that part is a bit dirty, 
public _asmProcessInit
_asmProcessInit:
	bl _processInit 
	
	; after processInit, r0 contains address of actual process entry point
	mov pc, r0
	