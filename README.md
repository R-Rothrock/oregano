# Oregano

PoC Linux process injection (process hollowing) to hide execution
of Linux binary in an `htop` process.

## Usage

The line of code where the payload to execute it defined like this.
```c
static char[] pathname = "./test.out";
```

After this, you can then compile and execute, and see the effects.

### Note

This program doesn't have any sort of error handling, as it is only
a proof-of-concept (PoC) that I stopped developing the moment it worked.

# Disclaimer

As I'm sure you know...

THIS PROGRAM COMES WITH ABSOLUTELY NO WARRANTY TO THE EXTENT ALLOWED BY APPLICABLE STATE AND FEDERAL LAW, etc.
---

See the MIT license for more details.
