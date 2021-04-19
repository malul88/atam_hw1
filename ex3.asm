.global _start

.section .text
_start: 
        xor %eax, %eax     # sum = 0
        xor %ecx, %ecx     # counter = 0
        movq $arr, %rbx
loop1:  cmp $0x0, (%rbx)   # check if arr"[i]" == 0
        jle calc
        add (%rbx), %eax   # sum += arr[i]
        inc %ecx           # counter++
        add $0x4, %rbx     # arr += 4
        jmp loop1
calc:   cmp $0x0,%ecx
        je end
        xor %rdx, %rdx
        div %ecx
end:    movl %eax ,avg
        
        
        