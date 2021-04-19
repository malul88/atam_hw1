.global _start

.section .text
_start:
        movq $source , %rax
        movq $destination, %rbx
        movl num, %ecx
        xor %dx, %dx
loop1:  cmp $0x0, %ecx
        jle end
        movb (%rax) ,%dl
        movb %dl, (%rbx)
        add $0x1, %rbx
        dec %ecx
        add $0x1, %rax
        jmp loop1
end:    