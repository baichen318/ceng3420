    la a0, AL          # cycle = 9 / 19, addr = 0x00000000
    lw a0, 0(a0)       # cycle = 29, a0 = -2 / 0xfffffffe, addr = 0x00000008
    blt a0, zero, L1   # cycle = 46, addr = 0x0000000c
L1  addi a7, a0, 13    # cycle = 56, a7 = 11 / 0x0000000b, addr = 0x00000010
    bge zero, a7, L1   # cycle = 66, addr = 0x00000014
	la a0, AL          # cycle = 76 / 86, addr = 0x00000018
    lb a1, 1(a0)       # cycle = 96, a1 = -1 / 0xffffffff, addr = 0x00000020
    xori a2, a1, 0x7FF # cycle = 113, a2 = -2048 / 0xfffff800, addr = 0x00000024
	lw a0, 0(a0)       # cycle = 123, a0 = -2 / 0xfffffffe, addr = 0x00000028
    add a3, a2, a0     # cycle = 140, a3 = -2050 / 0xfffff7fe, addr = 0x0000002c
    add a4, a2, a0     # cycle = 150, a4 = -2050 / 0xfffff7fe, addr = 0x00000030
    addi a5, a7, -1    # cycle = 160, a5 = 10 / 0x0000000a, addr = 0x00000034
    jal zero, L2       # cycle = 170, addr = 0x00000038
L3  jal ra, L4         # cycle = 192, addr = 0x0000003c
	jal zero, L5       # cycle = 224, addr = 0x00000040
L2  jal zero, L3       # cycle = 181, addr = 0x00000044
L4  sub a6, a7, a0     # cycle = 203, a6 = 13 / 0x0000000d, addr = 0x00000048
    jalr zero, 0(ra)   # cycle = 213, addr = 0x0000004c
L5  slli a7, a6, 3     # cycle = 235, a7 = 52 / 0x00000068, addr = 0x00000050
    srli a3, a7, 2     # cycle = 245, a3 = 26 / 0x0000001a, addr = 0x00000054
	sub a3, zero, a3   # cycle = 255, a3 = -26 / 0xffffffe6, addr = 0x00000058
    srai a4, a3, 2     # cycle = 265, a4 = -7 / 0xfffffff9, addr = 0x0000005c
    xori a3, a3, 0xF   # cycle = 275, a3 = -23 / 0xffffffe9, addr = 0x00000060
	sub a3, zero, a3   # cycle = 285, a3 = 23 / 0x00000017, addr = 0x00000064
	la s1, BL          # cycle = 295 / 305, addr = 0x00000068
    sb a3, 8(s1)       # cycle = 315, MEMORY[0x84 + 8] = 23, addr = 0x00000070
    xor a3, a3, a4     # cycle = 332, a3 = -18 / 0xffffffee, addr = 0x00000074
    sw a3, 16(s1)      # cycle = 342, MEMORY[0x84 + 16] = 0xffffffee, addr = 0x00000078
	halt               # cycle = 359, addr = 0x0000007c

AL   .FILL -2
BL   .FILL -9

