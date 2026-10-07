# SPDX-License-Identifier: GPL-2.0-or-later
# GNU Make with a POSIX shell (Linux, macOS, MSYS2, or WSL).
DASM ?= dasm

.PHONY: all clean
all:
	mkdir -p build
	"$(DASM)" main.asm -f3 -o"build/zipdrivesacar.bin" -l"build/zipdrivesacar.lst" -s"build/zipdrivesacar.sym"
	test "$$(wc -c < build/zipdrivesacar.bin)" -eq 4096

clean:
	rm -f build/zipdrivesacar.bin build/zipdrivesacar.lst build/zipdrivesacar.sym
