.global _start

.section .text
_start:
#your code here
    mov $1, %rax # new_len counter
    mov $0, %rbx # new_begin counter
    mov $0, %r10 # index i=0
    mov $arr, %r11 # d = &arr
    mov n, %r8d
    sub $1, %r8d # r8d = n-1 
    
    cmpl $0, n
    je returnZeros
    
    movl $1, len
    movl $0, begin
    
    loop_:
        
        cmpl %r8d, %r10d
        jge end
        
        movl (%r11,%r10,4), %ecx # ecx = arr[i]
        movl 4(%r11,%r10,4), %edx # edx = arr[i+1]
        
        cmpl %edx, %ecx # if arr[i] > arr[i+1]
        jg raiseLen
        
        movl $1, %eax
        movq %r10, %rbx
        addq $1, %rbx
        jmp nextLoop
        
    raiseLen:
        incl %eax
        movl len, %ecx
        cmpl len, %eax
        jg updateLen
        jmp nextLoop
    
    updateLen:
        movl %eax, len
        movl %ebx, begin
        
    nextLoop:
        incq %r10
        jmp loop_
    
    returnZeros:
        movl $0, len
        movl $0, begin
    
    end:
        movl len, %eax
        movl begin, %ebx
        ret
        