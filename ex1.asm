.global _start

.section .text
_start:
        mov num , %rax
        mov $0x1 ,%rbx
        xor %ecx, %ecx
    
loop1:  cmp $0x0, %rax
        jz end
        test %rbx, %rax
        jz nothing
        inc %ecx
nothing:shr $1, %rax
        jmp loop1
        
end: mov %ecx, countBits
