.global _start
.global shit

.section .data
msg: .ascii "Hello, please kill me.\n"
msg_end: .quad msg_end-msg

.section .bss

.section .text
    _start:
        # mov $1, %rdi
        # mov $msg, %rsi
        # mov msg_end, %rdx

        call shit
        jmp end

    shit:
        pushq %rbp
        movq %rsp, %rbp

        mov $1, %rax
        mov $1, %rdi
        mov $msg, %rsi
        mov (msg_end), %rdx
        syscall

        leave
        ret

    end:
        mov $60, %rax
        mov $0, %rdi
        syscall
        
    