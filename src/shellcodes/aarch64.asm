// aarch64.asm

.section .text
	.global _main

_main:

	mov x8, #221
	adr x0, _pathname

	eor x1, x1, x1
	eor x2, x2, x2

	svc #0

	mov x8, #93
	mov x0, #0

	svc #0

_pathname:
	.ascii "/bin/sh"
