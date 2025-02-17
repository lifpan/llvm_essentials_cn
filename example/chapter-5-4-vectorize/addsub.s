	.text
	.file	"addsub.ll"
	.globl	addsub
	.align	16, 0x90
	.type	addsub,@function
addsub:                                 # @addsub
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp0:
	.cfi_def_cfa_offset 16
.Ltmp1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp2:
	.cfi_def_cfa_register %rbp
	movl	b(%rip), %eax
	addl	c(%rip), %eax
	movl	%eax, a(%rip)
	movl	b+4(%rip), %eax
	addl	c+4(%rip), %eax
	movl	%eax, a+4(%rip)
	movl	b+8(%rip), %eax
	addl	c+8(%rip), %eax
	movl	%eax, a+8(%rip)
	movl	b+12(%rip), %eax
	addl	c+12(%rip), %eax
	movl	%eax, a+12(%rip)
	popq	%rbp
	retq
.Lfunc_end0:
	.size	addsub, .Lfunc_end0-addsub
	.cfi_endproc

	.type	a,@object               # @a
	.bss
	.globl	a
	.align	16
a:
	.zero	16
	.size	a, 16

	.type	b,@object               # @b
	.globl	b
	.align	16
b:
	.zero	16
	.size	b, 16

	.type	c,@object               # @c
	.globl	c
	.align	16
c:
	.zero	16
	.size	c, 16


	.ident	"clang version 3.8.1-15 (tags/RELEASE_381/final)"
	.section	".note.GNU-stack","",@progbits
