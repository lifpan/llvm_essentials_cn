	.text
	.file	"hadd.ll"
	.globl	hadd
	.align	2
	.type	hadd,@function
hadd:                                   // @hadd
// BB#0:
	mov		w8, v0.s[3]
	mov		w9, v0.s[2]
	add	 w8, w8, w9
	mov		w9, v0.s[1]
	add	 w8, w8, w9
	fmov	w9, s0
	add	 w0, w8, w9
	ret
.Lfunc_end0:
	.size	hadd, .Lfunc_end0-hadd


	.section	".note.GNU-stack","",@progbits
