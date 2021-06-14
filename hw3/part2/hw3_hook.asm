.global hook

.section .data
msg: .ascii "This code was hacked by Noa Killa's gang\n"
endmsg:

.section .text
hook:
  movq $_start, %rax # get address of _start (in a.o)
  add $0x1e, %rax # rax = address of first nop (in a.o)
  movb $0xe8, (%rax) # change code in a.o
  inc %rax

  movl $(_start-print_n+35), %edx
  imull $-1, %edx, %edx

  movl %edx, (%rax)
  jmp _start

print_n:
  # movq $60, %rax
  # movq $0, %rdi
  # syscall
  
  # print noa
  mov $1, %rax
  mov $1, %rdi
  mov $msg, %rsi
  mov $endmsg-msg, %rdx
  syscall
  ret
