
#define __ASSEMBLY__

.text

#ifdef __ELF__
#define fixed8_mul __fixed8_mul
#define fixed8_div __fixed8_div
#define fixed_mul __fixed_mul
#define fixed_div __fixed_div
#else
#define fixed8_mul ___fixed8_mul
#define fixed8_div ___fixed8_div
#define fixed_mul ___fixed_mul
#define fixed_div ___fixed_div
#endif

#define FIXED_SHIFT_SIZE $16
#define FIXED8_SHIFT_SIZE $24

.global fixed8_mul
.global fixed8_div

/*.global fixed_mul
.global fixed_div*/

fixed_mul: 
#define left 8(%ebp)
#define right 12(%ebp)
	pushl %ebp
	movl %esp, %ebp
	pushl %edx
	movl left, %eax
	movl right, %edx
	imull %edx
	shrdl FIXED_SHIFT_SIZE, %edx, %eax
	popl %edx
	movl %ebp, %esp
	popl %ebp
	ret

fixed_div: 
	pushl %ebp
	movl %esp, %ebp
	pushl %edx
	pushl %ebx

	movl left, %edx
	movl right, %ebx

	movl $0, %eax
	shrdl FIXED_SHIFT_SIZE, %edx, %eax
	sarl FIXED_SHIFT_SIZE, %edx
	idivl %ebx

	popl %ebx
	popl %edx
	movl %ebp, %esp
	popl %ebp
	ret

fixed8_mul: 
#define left 8(%ebp)
#define right 12(%ebp)
	pushl %ebp
	movl %esp, %ebp
	pushl %edx
	movl left, %eax
	movl right, %edx
	imull %edx
	shrdl $8, %edx, %eax
	popl %edx
	movl %ebp, %esp
	popl %ebp
	ret

fixed8_div: 
	pushl %ebp
	movl %esp, %ebp
	pushl %edx
	pushl %ebx

	movl left, %edx
	movl right, %ebx

	movl $0, %eax
	shrdl FIXED8_SHIFT_SIZE, %edx, %eax
	sarl FIXED8_SHIFT_SIZE, %edx
	idivl %ebx

	popl %ebx
	popl %edx
	movl %ebp, %esp
	popl %ebp
	ret
.end
