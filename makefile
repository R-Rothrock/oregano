#!/usr/bin/make

PREFIX := "/usr/local"

all: src/oregano.c
	gcc -o oregano src/oregano.c

install: oregano
	cp oregano $(PREFIX)/bin
