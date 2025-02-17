	.text
	.file	"hadd.ll"
	.globl	hadd
	.align	2
	.type	hadd,@function
hadd:                                   // @hadd
// BB#0:
	addv	s0, v0.4s
	fmov	w0, s0
	ret
.Lfunc_end0:
	.size	hadd, .Lfunc_end0-hadd


	.section	".note.GNU-stack","",@progbits
