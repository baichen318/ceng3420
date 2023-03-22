# /***************************************************************/
# /*                                                             */
# /*                  RISCV-LC Simulator                         */
# /*                                                             */
# /*                     CEG3420 Lab3                            */
# /*                 cbai@cse.cuhk.edu.hk                        */
# /*           The Chinese University of Hong Kong               */
# /*                                                             */
# /***************************************************************/

OS := $(shell uname -s)
SRC := $(shell find . -name "*.c")
TGT := riscv-lc
LIB := lib$(TGT).a
OPT := -Wall -std=c99 -Wno-return-type -O3


ifeq ($(OS), Darwin)
	lib_path = ./tools/macos
endif
ifeq ($(OS), Linux)
	lib_path = ./tools/linux
endif
ifeq (MINGW64, $(findstring MINGW64, $(OS)))
	lib_path = ./tools/win
	CC = gcc
endif

.default: all

all: $(TGT)

$(TGT): $(SRC)
	$(CC) $(OPT) $^ $(lib_path)/$(LIB) -o $@

.PHONY: clean
clean:
	rm -f $(TGT)
