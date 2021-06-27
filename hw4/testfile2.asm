.global _start
.global shit

.section .data
msg1: .ascii "Hello, please kill me.\n"
msg1_end: .quad msg1_end-msg1
msg2: .ascii "Shit...\n"
msg2_end: .quad msg2_end-msg2

.section .bss

.section .text
    _start:
        mov $1, %rax
        mov $1, %rdi
        mov $msg1, %rsi
        mov (msg1_end), %rdx
        syscall

        call shit

        mov $1, %rax
        mov $1, %rdi
        mov $msg1, %rsi
        mov (msg1_end), %rdx
        syscall

        jmp end

    shit:
        pushq %rbp
        movq %rsp, %rbp

        leave
        ret

    end:
        mov $60, %rax
        mov $0, %rdi
        syscall
        
    