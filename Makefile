
OPT := -Wall -std=c99 -Wno-return-type
SEL_OPT := -Wno-unused-variable

.default: all

all: sim

sim: lc_sim.c
	gcc $(OPT) $(SEL_OPT) $< -o $@

.PHONY: clean
clean:
	rm sim
