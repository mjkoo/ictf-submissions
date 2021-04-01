.text

.global __syscall
.type __syscall,@function
__syscall:
	movq %rdi,%rax
	movq %rsi,%rdi
	movq %rdx,%rsi
	movq %rcx,%rdx
	movq %r8,%r10
	movq %r9,%r8
	movq 8(%rsp),%r9
	syscall
	ret

.global payload
.type payload, @object
.align 8
payload: .quad . + 8

.incbin "decode-png_enc"

.global payload_size
.type payload_size, @object
.align 8
payload_size: .quad . - payload - 8

.section .tar, "aw" , @progbits

.global tar
.type tar, @object
.align 8
tar: .quad . + 8

.incbin "file.tar", 120

.global tar_size
.type tar_size, @object
.align 8
tar_size: .quad . - tar - 8
