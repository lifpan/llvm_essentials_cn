	.text
	.file	"output.bc"
	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
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
	subq	$16, %rsp
	movl	$0, -8(%rbp)
	movl	$2, %edi
	callq	add
	movl	%eax, %ecx
	movl	%ecx, -4(%rbp)
	movl	$.L.str, %edi
	xorl	%eax, %eax
	movl	%ecx, %esi
	callq	printf
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc

	.globl	add
	.p2align	4, 0x90
	.type	add,@function
add:                                    # @add
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp3:
	.cfi_def_cfa_offset 16
.Ltmp4:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp5:
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	addl	globvar(%rip), %edi
	movl	%edi, %eax
	popq	%rbp
	retq
.Lfunc_end1:
	.size	add, .Lfunc_end1-add
	.cfi_endproc

	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"%d\n"
	.size	.L.str, 4

	.type	globvar,@object         # @globvar
	.data
	.globl	globvar
	.p2align	2
globvar:
	.long	12                      # 0xc
	.size	globvar, 4


	.ident	"clang version 3.8.1-15 (tags/RELEASE_381/final)"
	.ident	"clang version 3.8.1-15 (tags/RELEASE_381/final)"
	.section	".note.GNU-stack","",@progbits
