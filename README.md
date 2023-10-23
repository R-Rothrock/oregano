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

## Contributing

Do so. Or don't. Up to you. You will get special thanks and stuff if you do.

### Note

It doesn't work _perfectly_. Sometimes, the _tracee_ segfaults or fails to
execute the shellcode properly (though this could be a problem with the
shellcode.) It generally works anywhere between %5 and %95. This usuallydepends
on the executable being injected. It has been confirmed that injecting an
executable that has the `execstack` header flag is far more likely to be
injected successfully (%90-100.)

# As I'm sure you know...

THIS PROGRAM COMES WITH ABSOLUTELY NO WARRANTY TO THE EXTENT ALLOWED BY 
APPLICABLE STATE AND FEDERAL LAW, etc.
---

See the MIT license for more details. Basically if you decide to be a dumbass I'm
not affected.
