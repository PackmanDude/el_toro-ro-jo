#!/bin/sh

app='Eltororojo'
gcc ${app}.c -O2 -lSDL2 -lvulkan -o $app
