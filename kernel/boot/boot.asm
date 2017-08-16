;*******************************************************************************
; 				Boot code
;*******************************************************************************


;
; declare functions from other files
;extern _krn_preboot
;extern _krn_init

.text

; When an interrupt occurs:
; - Execution context changes to fixed address 0x8
; - PC is set to the Interrupt Handler
; - Cpu Mode set to Supervisor
; - IRQs are disabled
; - Some registers are set:
;		lr - The interrupted context
;		ip - Bus and Reason for the interrupt: (bus << 24) | reason
;       r0,r1,r2,r3 - Values dependent on the interrupt type.
;
;*******************************************************************************

; 0x0 points to boot function, boot function will overwrite this with interrupt handler
_intrHandlerAddr:
.word _intrHandler_Reset
.word _intrHandler_All

; This is the default execution context
public _mainCpuCtx
_mainCpuCtx:
	.zero 16 ; 4*4, zeroed anyway
	.word 0x12345678 ; set dummy in r4
	.zero 44 ; zero 4*11, r5-r15
	
	.word 0 ; flags register
	.zero 128 ; floating point registers (16*8)
	; Marker so we can check when booting if the interrupt context matches the
	; size of what we have in C source code
	public _mainCpuCtxEnd
	_mainCpuCtxEnd:

extern _kernelInit
extern _kernelHandleInterrupt;

;
; minimal boot code, rest in C code
;
_intrHandler_Reset:
	str [_kernelCurrIntrBusAndReason], 0
	str [_kernelPrevIntrBusAndReason], 0
	
	bl _kernelInit
	; kernel init has setup new context, and returns
	; it's address, let's switch to it
	ctxswitch [r0]

	; We will only get here if some other context changes to our context, which
	; per design, we don't allow
	;extern _krn_panicUnexpectedCtxSwitch
	;b _krn_panicUnexpectedCtxSwitch	

superloop1:	
	b superloop1
;
; minimal interrupt handler (save the cause and pass control to C code)
;
_intrHandler_All:
	; Save the interrupted context
	str [_kernelInterruptedContext], lr
	
	; Set the previous bus and reason variable
	; This allows us to detect kernel double faults
	ldr r4, [_kernelCurrIntrBusAndReason]
	str [_kernelPrevIntrBusAndReason], r4
	str [_kernelCurrIntrBusAndReason], ip
	
	; rest are in registers r0-r3
	
	bl _kernelHandleInterrupt
	
	; We are done with the interrupt servicing, so mark it as so.
	str [_kernelCurrIntrBusAndReason], -1
	
	ctxswitch [r0]
	
	; We will only get here if some other context changes to our context, which
	; per design, we don't allow

superloop2:	
	b superloop2
;	b _krn_panicUnexpectedCtxSwitch
	

;*******************************************************************************
;*******************************************************************************
;*******************************************************************************

;
; Global variables with misc system information
;
.bss

; These two variables allows detection of double faults.
; When we are serving an interrupt, if another one happens, it's a double fault.
public _kernelCurrIntrBusAndReason
_kernelCurrIntrBusAndReason:
.word 15
public _kernelPrevIntrBusAndReason
_kernelPrevIntrBusAndReason:
.word 15

; Set when an interrupt occurs
public _kernelInterruptedContext
_kernelInterruptedContext:
.word 0

;
; Read only data (after we set MMU)
;
.rodata

;
; Process runtime information
; This is patched by the linker when building a ROM file, 
; and information about the size of the program:
; Contents are:
; 	4 bytes - readOnlyAddress (where code and .rodata starts)
;	4 bytes - readOnlySize (size of the read only portion)
;	4 bytes - readWriteAddress ( where read/write data starts)
;	4 bytes - readWriteSize (size of the read/write data)
;	4 bytes - sharedReadWriteAddress ( where shared read/write data starts)
;	4 bytes - sharedReadWriteSize (size of the shared read/write data)
public _processInfo
_processInfo:
.zero 24

.section ".apcpudebug"


