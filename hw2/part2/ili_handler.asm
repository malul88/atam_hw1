.globl my_ili_handler

.text
.align 4, 0x90
my_ili_handler: # i don't think prologue and epilogue are needed here because this code uses the kernel stack (might lead to error if we use them)
  # save registers (kernel must only change %rdi)

  # jmp sleep

  pushq %rbp
  movq %rsp, %rbp

  pushq %rdx
  pushq %rcx
  pushq %rsi
  pushq %rax
  pushq %rdi

  # jmp sleep

  # get opcode
  xorq %rdx, %rdx
  movq 8(%rbp), %rcx # get old %rip ---------------------------- need to check because error code is OPTIONAL: (simu-lev 2)

  # jmp sleep
  movq (%rcx), %rdx # dx now holds 2 bytes - for the opcode

  

  # check which instruction led to interrupt
  xorq %rdi, %rdi
  movq $1, %rsi
  movb %dl, %dil # rdi lower byte now holds first byte of opcode
  cmpb $0x0F, %dl # check lower byte of opcode is 0x0f
  jne call_what # can move to calling func
  movw %dx, %di
  shr $8, %di
  # movb %dh, %dil # put second byte of opcode into rdi
  movq $2, %rsi

  # call what_to_do with the last byte of opcode
  call_what:
  pushq %rdi
  pushq %rsi
  call what_to_do
  popq %rsi
  popq %rdi

  cmpl $0, %eax
  je what_ret_zero

  # in case returned != 0, prog needs to keep running, meaning rip+ rsi (?)
  movl %eax, %edi
  addq %rsi, 8(%rbp) # update rip to point to next instruction
  jmp end

  # in case returned == 0, original handler needs to sort it
  what_ret_zero:
  popq %rdi
  popq %rax
  popq %rsi
  popq %rcx
  popq %rdx
  leave
  jmp *old_ili_handler
  
  end: 
  # restore changed registers (other than rdi) and return
  addq $8, %rsp
  popq %rax
  popq %rsi
  popq %rcx
  popq %rdx
  leave
  iretq

  sleep:
  pushq $0     # 0 nanoseconds
  pushq $10    # 10 seconds

  # RSP instead of RBP in the next instruction:
  mov   %rsp, %rdi    # the time structure we just pushed
  mov   $35, %eax     # SYS_nanosleep
  xor   %esi, %esi    # rem=NULL, we don't care if we wake early
  syscall
  # RSP is 16 bytes lower than it was before this fragment, in case that matters for later code.
