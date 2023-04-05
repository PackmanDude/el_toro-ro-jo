#!/bin/sh

app='Eltororojo'
if [ "$1" = dev ]; then
	gcc -Wall -Wextra -Wpedantic -Wformat=2 -Wno-unused-parameter \
		-Wshadow -Wwrite-strings -Wstrict-prototypes \
		-Wold-style-definition -Wredundant-decls -Wnested-externs \
		-Wmissing-include-dirs -Wjump-misses-init -Wlogical-op \
		-g -lSDL2 -lvulkan $app.c -o $app
else
	gcc -O2 -lSDL2 -lvulkan $app.c -o $app
fi
