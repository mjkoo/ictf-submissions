# debug me not

This program is filled with so many bugs that I can't find the flag :(

(`./crackme FLAG` should print "you win!")

## Writeup

This program contains some code which decodes a constant string into a stack buffer and then compares it to user input (from argv).
The intended approach is to use a debugger to examine the arguments to `strncmp` to determine the flag value.

There are a few tricks to this:

1. The code which does the decoding is XOR-encoded in the .data section, and copied to a RWE region during runtime to be executed.
1. The decoding step happens in two phases, one is obvious inside of main, the other is in a function (`__init_plt`) declared with `__attribute__((constructor))` which runs prior to main.
1. The code inside `__init_plt` uses `ptrace(PTRACE_TRACEME, ...)` to determine if it's being run inside a debugger, and fails to do its decoding if so.

A gdb script which can be used to find the flag:

```
# ptrace(PTRACE_TRACEME) call
b *0x401045
# strncmp call
b *0x401331

# run it
r ictf

# at ptrace
ni
set $rax=0
c

# at strncmp
x/s $rdi
```
