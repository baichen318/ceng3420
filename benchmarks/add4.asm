# finally, BL will be changed from -5 to -1
	la a1, AL
	lw a1, 0(a1)
	la a2, BL
	lw a2, 0(a2)
LOOP addi a1, a1, -1
	addi a2, a2, 1
	bne a1, zero, LOOP
	la a3, BL
	sw a2, 0(a3)
	halt

AL   .FILL 4
BL   .FILL -5

