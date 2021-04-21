
OPT := -Wall -std=c99 -Wno-return-type
SEL_OPT := -Wno-unused-variable
SL := lib.a

.default: all

all: lc

lc: riscv-lc.c util.c $(SL)
	gcc $(OPT) $(SEL_OPT) $^ -o $@

.PHONY: clean
clean:
	rm lc
