# Oregano

PoC Linux process injection (process hollowing) to hide execution
of Linux binary in an `htop` process.

## Usage

```
OPTIONS: ./oregano [PID/PATH] [EXECUTABLE]
```

The first argument defines the `tracee` either by PID or program to
execute, and the second argument defines what you would like to be
executed _by_ the shellcode. I will add support for `.bin` files to be
supplied for shellcode in the future, but feel free to send a
PR if you want to do it, which brings me to

## Contributing

Do so. Or don't. Up to you. You will get special thanks and stuff if you do.

### Note

It doesn't work _perfectly_. Sometimes, the _tracee_ segfaults or does
something like this:

```

 .ei88ie.
.fE'  'Gf
i8      8i
D8      8L
D8.    .8I
:GL    j8'
 't8jj9t'reg@n0
 --------------
 : Author: Roan Rothrock
 : <roan.rothrock@hotmail.com>
[-] Child PID: 14393
[-] Attached to process 14393. Preparing shellcode.
[+] Shellcode size: 67
[+] Shellcode: 
0x48 0x31 0xc0 0x48 0x31 0xff 0x48 0x31 0xf6 0x48 
0x31 0xd2 0x48 0x8d 0x3d 0x37 0x10 0x10 0x10 0x48 
0x81 0xef 0x10 0x10 0x10 0x10 0x48 0x8d 0x35 0x29 
0x10 0x10 0x10 0x48 0x81 0xee 0x10 0x10 0x10 0x10 
0x56 0x48 0x89 0xe6 0xb0 0x3b 0x0f 0x05 0x48 0x31 
0xc0 0xb0 0x3c 0x48 0x31 0xff 0x0f 0x05 0x2f 0x62 
0x69 0x6e 0x2f 0x68 0x74 0x6f 0x70 
[+] Instruction pointer (RIP/EIP): 0x7fc68c4da196
[-] Injecting shellcode...
[-] Shellcode injected. Restarting process.
[-] Child exited. Exiting...
[roanr@manjaro oregano]$ Error: unsupported non-option ARGV-elements: A�ą���
```

So, it's not exactly safe (nor do I think it will ever truly be) but it works _most_ of the time.

# As I'm sure you know...

THIS PROGRAM COMES WITH ABSOLUTELY NO WARRANTY TO THE EXTENT ALLOWED BY 
APPLICABLE STATE AND FEDERAL LAW, etc.
---

See the MIT license for more details. Basically if you're a dumbass I'm
not affected (thank God.)
