.global _start

.section .data
msg1: .ascii "Hello, please kill me.\n"
msg1_end: .quad msg1_end-msg1

.section .bss

.section .text
    _start:
        mov $1, %rax
        mov $1, %rdi
        mov $msg1, %rsi
        mov (msg1_end), %rdx
        syscall

        mov $60, %rax
        mov $0, %rdi
        syscall
