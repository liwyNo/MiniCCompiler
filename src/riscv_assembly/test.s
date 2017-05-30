	.file	"test.c"
	.option nopic
	.comm	a,320,4
	.comm	b,4,4
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	add	sp,sp,-16
	sw	s0,12(sp)
	add	s0,sp,16
	li	a5,0
	mv	a0,a5
	lw	s0,12(sp)
	add	sp,sp,16
	jr	ra
	.size	main, .-main
	.ident	"GCC: (FreeBSD Ports Collection for riscv64) 6.1.0"
