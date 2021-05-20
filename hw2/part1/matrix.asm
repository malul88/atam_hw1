.global get_elemnt_from_matrix, multiplyMatrices
.extern set_elemnt_in_matrix

.section .text
get_elemnt_from_matrix: # rdi - &matrix, esi - n, edx - row, ecx - col
	# epilogue
	pushq %rbp
	movq %rsp, %rbp
	
	# body
	xorq %rax, %rax
	xorq %r11, %r11
	movl %ecx, %r11d
	movq %r11, %rcx

	movl %edx, %eax # eax = row
	mull %esi # edx:eax = n*row
	imull $4, %eax, %eax # eax = 4*n*row
	addq %rax, %rdi # rdi = &matrix + 4*n*row
	movq (%rdi, %rcx, 4), %rax # *((matrix + n*row*4) + 4*col) = rax
	
	# prologue
	leave
	ret

multiplyMatrices: 
	# epilogue
	pushq %rbp
	movq %rsp, %rbp
	subq $44, %rsp # for local vars
	
	# save local vars
	movq %rdi, -8(%rbp) # &first
	movq %rsi, -16(%rbp) # &second
	movq %rdx, -24(%rbp) # &result
	movl %ecx, -28(%rbp) # m (row of first)
	movl %r8d, -32(%rbp) # n (col of first)
	movl %r9d, -36(%rbp) # r (col of second)
	movl $0, -40(%rbp) # index i
	movl $0, -44(%rbp) # index j
	# 16(rbp) = p (p of Zp)

	# body
	loop_i:
		# check i == m
		movl -28(%rbp), %edx # m
		cmp -40(%rbp), %edx # i
		je end_loop_i
		movl $0, -44(%rbp) # j

		loop_j:
			# check j == r
			movl -44(%rbp), %edx # j
			cmp -36(%rbp), %edx # r
			je end_loop_j

			movq -8(%rbp), %rdi # &first
			movq -16(%rbp), %rsi # &second
			movl -32(%rbp), %edx # n
			movl -36(%rbp), %ecx # r
			movl -40(%rbp), %r8d # i
			movl -44(%rbp), %r9d # j
			call mulRowByCol
			# eax now hold the result for rowAi*ColBj

			xorq %rdx, %rdx
			movl 16(%rbp), %ecx # p
			div %ecx # edx:eax/p
			
			# change result[i,j]  to edx
			movl %edx, %r8d # edx - value
			movq -24(%rbp), %rdi # &result
			movl -36(%rbp), %esi # r - col of result
			movl -40(%rbp), %edx # i - index of row
			movl -44(%rbp), %ecx # j - index of col
			call set_elemnt_in_matrix

			incl -44(%rbp) # j
			jmp loop_j
		
		end_loop_j:
			incl -40(%rbp) # i
			jmp loop_i
	
	end_loop_i:
	# prologue
	leave
	ret

mulRowByCol: # int mulRowCol(int* mat1, int* mat2, int n, int r, int i, int j)
	# epilogue
	pushq %rbp
	movq %rsp, %rbp
	subq $36, %rsp

	# save local vars
	movq %rdi, -8(%rbp) # &first
	movq %rsi, -16(%rbp) # &second
	movl %edx, -20(%rbp) # n - col of first
	movl %ecx, -24(%rbp) # r - col of second
	movl %r8d, -28(%rbp) # i - row of first
	movl %r9d, -32(%rbp) # j - row of second
	movl $0, -36(%rbp) # k - index
	
	xorq %r11, %r11

	# save local vars
	loop_k:
		# check k == n
		movl -36(%rbp), %edx # k
		cmp -20(%rbp), %edx # n
		je end_loop_k

		movq -8(%rbp), %rdi # &first
		movl -20(%rbp), %esi # n
		movl -28(%rbp), %edx # i
		movl -36(%rbp), %ecx # k
		pushq %r11
		call get_elemnt_from_matrix
		popq %r11
		movl %eax, %r10d # r10d = mat1[i,k]

		movq -16(%rbp), %rdi # &second
		movl -24(%rbp), %esi # r
		movl -36(%rbp), %edx # k
		movl -32(%rbp), %ecx # j
		pushq %r11
		call get_elemnt_from_matrix
		popq %r11
		
		mul %r10d # edx:eax = mat1[i,k]*mat2[k,j]
		add %eax, %r11d
		
		incl -36(%rbp) # k
		jmp loop_k
	
	end_loop_k:
		movl %r11d, %eax
	
	# prologue
	leave
	ret
