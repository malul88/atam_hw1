.global _start



.section .text
_start:
#your code here
  
    movq head, %rbx
    movq head, %rax

    movq src, %r8
    movq dst, %r9
    
    loop_1:
        cmpq $0, %rbx # if head == nullptr exit
        je end
        cmpq %r9, (%rbx) # if dst before src exit
        je end
        
        cmpq %r8, (%rbx) # if src before dst continue
        je srcFound
        
        movq %rbx, %rax # %a is now the last pointer
        movq 8(%rbx), %rbx # %b is now the current pointer
        jmp loop_1
        
    srcFound:
        movq %rbx, %rcx
        movq 8(%rbx), %rdx
        
    loop_2:
        cmpq $0, %rdx # if next == nullptr exit
        je end
        cmpq %r9, (%rdx) # if dst found continue
        je swap
        
        movq %rdx, %rcx # %c is now the last pointer
        movq 8(%rdx), %rdx # %d is now the current pointer
        jmp loop_2
    
    swap:
        cmp %rax , %rbx    # check if head points to src (private case)
        je head_p
        movq %rdx,8(%rax)  # address of dst to pointer of src
        movq %rbx, 8(%rcx) # address of src to pinter of dst
        
        movq 8(%rdx), %r10 # r10 = dst_next
        movq 8(%rbx), %r11 # r11 = src_next
        movq %r11, 8(%rdx) # dst_next <- src_next
        movq %r10, 8(%rbx) # src_next<- dst-next
        jmp end
        
        
    head_p:    
        movq %rdx, head    # address of dst to head
        movq %rbx, 8(%rcx) # address of src to dst
        
        movq 8(%rdx), %r10 # r10 = dst_next
        movq 8(%rbx), %r11 # r11 = src_next
        movq %r11, 8(%rdx) # dst_next <- src_next
        movq %r10, 8(%rbx) # src_next<- dst-next
        
    end:
      

    