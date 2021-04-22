# This program counts from 10 to 0, the result is in t2
# t2 = 55 / 0x00000037
    la t0, ten
    lw t1, 0(t0)
    add t2, zero, zero
loop add t2, t2, t1
    addi t1, t1, -1
    bne t1, zero, loop
    halt

ten .fill 10
