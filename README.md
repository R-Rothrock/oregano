# Oregano

PoC Linux process injection (process hollowing) to hide execution
of Linux binary in an `htop` process.

## Usage

```
OPTIONS: oregano [PID/PATH] [EXECUTABLE]
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
something weird. If you want to see what I'm talking about, try `sudo oregano 1 /usr/bin/reboot`

So, it's not exactly safe (nor do I think it will ever truly be) but it works _most_ of the time.

# As I'm sure you know...

THIS PROGRAM COMES WITH ABSOLUTELY NO WARRANTY TO THE EXTENT ALLOWED BY 
APPLICABLE STATE AND FEDERAL LAW, etc.
---

See the MIT license for more details. Basically if you're a dumbass I'm
not affected (thank God.)
