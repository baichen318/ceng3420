
OPT := -Wall -std=c99 -Wno-return-type
SEL_OPT := -Wno-unused-variable

DISTRIBUTION = $(shell lsb_release -s)
ifeq ($(findstring "CentOS", $(DISTRIBUTION)), "CentOS")
	SL := libs/libcentos.a
else
	SL := libs/libubuntu.a
endif

.default: all

all: lc

lc: riscv-lc.c util.c $(SL)
	gcc $(OPT) $(SEL_OPT) $^ -o $@

