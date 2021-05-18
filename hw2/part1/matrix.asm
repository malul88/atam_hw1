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
	subq $64, %rsp # for local vars
	
	# save local vars
	movq %rdi, -8(%rbp) # &first
	movq %rsi, -16(%rbp) # &second
	movq %rdx, -24(%rbp) # &result
	movq %rcx, -32(%rbp) # m (row of first)
	movq %r8, -40(%rbp) # n (col of first)
	movq %r9, -48(%rbp) # r (col of second)
	movq $0, -56(%rbp) # index i
	movq $0, -64(%rbp) # index j
	# 16(rbp) = p (p of Zp)

	# body
	loop_i:
		# check i == m
		movq -32(%rbp), %rdx
		cmp -56(%rbp), %rdx
		je end_loop_i

		loop_j:
			# check j == r
			movq -64(%rbp), %rdx
			cmp -48(%rbp), %rdx
			je end_loop_j

			movq -8(%rbp), %rdi
			movq -16(%rbp), %rsi
			movq -40(%rbp), %rdx
			movq -48(%rbp), %rcx
			movq -56(%rbp), %r8
			movq -64(%rbp), %r9
			call mulRowByCol
			# rax now hold the result for rowAi*ColBj

			xorq %rdx, %rdx
			movq 16(%rbp), %rcx
			div %rcx # rdx:rax/p
			
			# change result[i,j]  to rdx
			movq %rdx, %r8 # rdx - value
			movq -24(%rbp), %rdi # &result
			movq -48(%rbp), %rsi # r - col of result
			movq -56(%rbp), %rdx # i - index of row
			movq -64(%rbp), %rcx # j - index of col
			call set_elemnt_in_matrix

			incq -64(%rbp)
			jmp loop_j
		
		end_loop_j:
			incq -56(%rbp)
			jmp loop_i
	
	end_loop_i:
	# prologue
	leave
	ret

mulRowByCol: # int mulRowCol(int* mat1, int* mat2, int n, int r, int i, int j)
	# epilogue
	pushq %rbp
	movq %rsp, %rbp
	subq $56, %rsp

	# save local vars
	movq %rdi, -8(%rbp) # &first
	movq %rsi, -16(%rbp) # &second
	movq %rdx, -24(%rbp) # n - col of first
	movq %rcx, -32(%rbp) # r - col of second
	movq %r8, -40(%rbp) # i - row of first
	movq %r9, -48(%rbp) # j - row of second
	movq $0, -56(%rbp) # k - index
	
	xorq %r11, %r11

	# save local vars
	loop_k:
		# check k == n
		movq -56(%rbp), %rdx
		cmp -24(%rbp), %rdx
		je end_loop_k

		movq -8(%rbp), %rdi # &first
		movq -24(%rbp), %rsi # n
		movq -40(%rbp), %rdx # i
		movq -56(%rbp), %rcx # k
		call get_elemnt_from_matrix
		movq %rax, %r10 # r10 = mat1[i,k]

		movq -16(%rbp), %rdi # &second
		movq -32(%rbp), %rsi # r
		movq -56(%rbp), %rdx # k
		movq -48(%rbp), %rcx # j
		call get_elemnt_from_matrix
		
		mul %r10 # rdx:rax = mat1[i,k]*mat2[k,j]
		add %rax, %r11
		
		incq -56(%rbp)
		jmp loop_k
	
	end_loop_k:
		movq %r11, %rax
	
	# prologue
	leave
	ret
