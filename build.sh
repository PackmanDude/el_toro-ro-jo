#!/bin/sh

APP=Eltororojo
ARCH=x86_64
EXT=elf
LIBS='-lSDL2 -lSDL2_image -lvulkan'

cd "$(dirname "$0")"
if [ "$1" = dev ]; then
	gcc -Wall -Wextra -Wpedantic -Wformat=2 -Wno-unused-parameter \
		-Wshadow -Wwrite-strings -Wstrict-prototypes \
		-Wold-style-definition -Wredundant-decls -Wnested-externs \
		-Wmissing-include-dirs -Wjump-misses-init -Wlogical-op \
		-g $LIBS $APP.c -o $APP-$ARCH-debug.$EXT
elif [ "$1" = clean ]; then
	rm -fv $APP*.$EXT
else
	gcc -O2 $LIBS $APP.c -o $APP-$ARCH.$EXT
fi
cd - >/dev/null
