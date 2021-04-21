
OPT := -Wall -std=c99 -Wno-return-type
SEL_OPT := -Wno-unused-variable
SL := libs/libdarwin.a

.default: all

all: lc

lc: riscv-lc.c util.c $(SL)
	gcc $(OPT) $(SEL_OPT) $^ -o $@

