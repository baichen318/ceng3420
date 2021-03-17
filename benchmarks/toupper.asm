# convert a null-terminated character string to upper case
	la a0, RSTART
    lw a0, 0(a0)
    la a1, WSTART
    lw a1, 0(a1)

LOOP lb a2, 0(a0)
    beq a2, zero, EXIT
    subi a2, a2, 32
WRITE sb a2, 0(a1)
    addi a0, a0, 1
    addi a1, a1, 1

EXIT sb a2, 0(a1)

RSTART .fill 0x4000
WSTART .fill 0x4002
