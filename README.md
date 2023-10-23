# Oregano

PoC Linux process injection with a working command line interface.

## Requirements
- Linux x86-64 >= 3.something (I don't remember exactly)
- Proper `ptrace_scope` permissions (Google it if you're not sure.)

## Installation
```shell
$ git clone https://github.com/R-Rothrock/oregano
$ cd oregano
$ make
$ ./oregano
...
```

## Usage

```
OPTIONS: oregano [PID/PATH] [EXECUTABLE/BIN]
```

The first argument defines the `tracee` either by PID or program to
execute, and the second argument defines whether you want your own shellcode,
or whether you want _my_ built-in shellcode to execute a file of your choosing.

### Note

This program doesn't work with a 100% success rate. To confirm that it works, use
short shellcode (preferably the shellcode built in, as opposed to using your own.) 
Additionally, injecting a program with the `execstack` executable flag will have a
100% success rate ([thanks to Mladia for bringing this up](https://github.com/R-Rothrock/oregano/issues/1).)

# As I'm sure you know...

THIS PROGRAM COMES WITH ABSOLUTELY NO WARRANTY TO THE EXTENT ALLOWED BY 
APPLICABLE STATE AND FEDERAL LAW, etc.
---

See the MIT license for more details. Basically if you decide to be a dumbass I'm
not affected.
