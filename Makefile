OS = $(shell uname -s)
ifeq ($(OS), Linux)
		-include linux.mk
endif

ifeq ($(OS), Darwin)
		-include darwin.mk
endif

.PHONY: clean
clean:
	rm lc
