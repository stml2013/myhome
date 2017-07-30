	.file	"test.c"
	.section	.rodata
.LC0:
	.string	"hello"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	andl	$-16, %esp
	subl	$16, %esp
	movl	__gcov0.main, %eax
	movl	__gcov0.main+4, %edx
	addl	$1, %eax
	adcl	$0, %edx
	movl	%eax, __gcov0.main
	movl	%edx, __gcov0.main+4
	movl	$.LC0, (%esp)
	call	puts
	movl	$0, %ecx
	movl	__gcov0.main+8, %eax
	movl	__gcov0.main+12, %edx
	addl	$1, %eax
	adcl	$0, %edx
	movl	%eax, __gcov0.main+8
	movl	%edx, __gcov0.main+12
	movl	%ecx, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.local	__gcov0.main
	.comm	__gcov0.main,16,8
	.type	_GLOBAL__sub_I_65535_0_main, @function
_GLOBAL__sub_I_65535_0_main:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	movl	$.LPBX0, (%esp)
	call	__gcov_init
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	_GLOBAL__sub_I_65535_0_main, .-_GLOBAL__sub_I_65535_0_main
	.section	.init_array,"aw"
	.align 4
	.long	_GLOBAL__sub_I_65535_0_main
	.data
	.align 4
	.type	__gcov_.main, @object
	.size	__gcov_.main, 24
__gcov_.main:
	.long	.LPBX0
	.long	1
	.long	1021627188
	.long	2092124921
	.long	2
	.long	__gcov0.main
	.section	.rodata
	.align 4
.LC1:
	.string	"/home/wyp/\346\241\214\351\235\242/testNx/code_test/make_moluo/test.gcda"
	.data
	.align 32
	.type	.LPBX0, @object
	.size	.LPBX0, 56
.LPBX0:
	.long	875575338
	.long	0
	.long	1760861229
	.long	.LC1
	.long	__gcov_merge_add
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	1
	.long	.LPBX1
	.align 4
	.type	.LPBX1, @object
	.size	.LPBX1, 4
.LPBX1:
	.long	__gcov_.main
	.ident	"GCC: (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1"
	.section	.note.GNU-stack,"",@progbits
