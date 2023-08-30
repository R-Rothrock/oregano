#!/usr/bin/make

all:
	nasm -felf64 x86_64.asm
	ld x86_64.o
	rm x86_64.o

bin:
	nasm -f bin x86_64.asm -o x86_64.bin
