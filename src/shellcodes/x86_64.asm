; x86_64.asm

section .text
	global _start

_start:

call _instructions

_instructions:

	xor rax, rax
	xor rdi, rdi
	xor rsi, rsi
	xor rdx, rdx

	; getting address of `pathname`
	pop rdi
	add rdi, 0x69

	mov al, 59

	mov rdi, rsp

	syscall

	mov al, 60

	xor rdi, rdi

	syscall

_pathname:
	db "/usr/bin/shutdown",0
