# Oregano

PoC Linux process injection (process hollowing) to hide execution
of Linux binary in an `htop` process.

## Usage

```
$ ./oregano 
OPTIONS: ./oregano [PID/PATH] [EXECUTABLE]
```

### Note

It doesn't work _perfectly_. For example, if `RIP`/`EIP` is nearing the
end of the executable, the shellcode will overflow and cause it to
segfault. There's plenty of problems like these, but it still works
sufficiently.

# As I'm sure you know...

THIS PROGRAM COMES WITH ABSOLUTELY NO WARRANTY TO THE EXTENT ALLOWED BY 
APPLICABLE STATE AND FEDERAL LAW, etc.
---

See the MIT license for more details. Basically if you're a dumbass I'm
not affected (thank God.)
