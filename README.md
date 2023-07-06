# Oregano

PoC Linux process injection (process hollowing) to hide execution
of Linux binary via `execve` syscall from injected shellcode.

# Usage

You'll have to change the line of code at the begining that looks
like this:

```c
static char[] pathname = "./test.out";
```

After this, you can then compile and execute, and see the effects.

# Disclaimer

As I'm sure you know...

THIS PROGRAM COMES WITH ABSOLUTELY NO WARRANTY TO THE EXTENT ALLOWED BY APPLICABLE STATE AND FEDERAL LAW, etc.
---

See the MIT license for more details.
