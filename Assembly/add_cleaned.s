	.file	"add.cpp"
	.text
	.globl	_Z11add_a_and_bii
	.type	_Z11add_a_and_bii, @function
_Z11add_a_and_bii:
	endbr64
	pushq	%rbp
	movq	%rsp, %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %eax
	addl	%edx, %eax
	popq	%rbp
	ret
	.globl	main
	.type	main, @function
main:
	endbr64
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$3, %esi
	movl	$2, %edi
	call	_Z11add_a_and_bii
	nop
	popq	%rbp
	ret
