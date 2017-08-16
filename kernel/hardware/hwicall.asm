.text

public _hwiCall_1_0
_hwiCall_1_0:
	push {lr}
	; setup call to hwi.
	mov ip, r0
	mov r0, r1
	hwi
	mov r0, ip
	pop {pc}

public _hwiCall_2_0
_hwiCall_2_0:
	push {lr}
	; setup call to hwi.
	mov ip, r0
	mov r0, r1
	mov r1, r2
	hwi
	mov r0, ip
	pop {pc}
	
public _hwiCall_3_0
_hwiCall_3_0:
	push {lr}
	; setup call to hwi.
	mov ip, r0
	mov r0, r1
	mov r1, r2
	mov r2, r3
	hwi
	mov r0, ip
	pop {pc}
	
	
public _hwiCall_0_1
_hwiCall_0_1:
	push {lr}
	mov ip, r0
	hwi
	pop {pc}

public _hwiCall_0_2
_hwiCall_0_2:
	push {r4,lr}
	mov r4,r1
	; setup call to hwi.
	mov ip, r0
	hwi
	str [r4 + 4*0], r0
	str [r4 + 4*1], r1
	mov r0, ip

	pop {r4,pc}
	
public _hwiCall_0_4
_hwiCall_0_4:
	push {r4,lr}
	mov r4,r1
	; setup call to hwi.
	mov ip, r0
	hwi
	str [r4 + 4*0], r0
	str [r4 + 4*1], r1
	str [r4 + 4*2], r2
	str [r4 + 4*3], r3
	mov r0, ip

	pop {r4,pc}
	