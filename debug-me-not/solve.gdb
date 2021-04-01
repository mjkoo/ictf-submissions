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
