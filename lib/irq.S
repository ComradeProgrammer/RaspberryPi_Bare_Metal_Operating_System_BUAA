
.globl enable_irq
enable_irq:
	msr    daifclr, #2 
	ret

.globl disable_irq
disable_irq:
	msr	daifset, #2
	ret
.globl put32
put32:
	str w1,[x0]
	ret

.globl get32
get32:
	ldr w0,[x0]
	ret
.global env_pop_tf
env_pop_tf:
	mov x16,x0
	msr ttbr0_el1,x1
	ldp	x0, x1, [x16, #16 * 0]
	ldp	x2, x3, [x16, #16 * 1]
	ldp	x4, x5, [x16, #16 * 2]
	ldp	x6, x7, [x16, #16 * 3]
	ldp	x8, x9, [x16, #16 * 4]
	ldp	x10, x11, [x16, #16 * 5]
	ldp	x12, x13, [x16, #16 * 6]
	ldp	x14, x15, [x16, #16 * 7]
	//ldp	x16, x17, [x16, #16 * 8]
	ldp	x18, x19, [x16, #16 * 9]
	ldp	x20, x21, [x16, #16 * 10]
	ldp	x22, x23, [x16, #16 * 11]
	ldp	x24, x25, [x16, #16 * 12]
	ldp	x26, x27, [x16, #16 * 13]
	ldp	x28, x29, [x16, #16 * 14]
	ldr	x30, [x16, #16 * 15] 
	
	ldr x17,[x16,#8*31]
	msr sp_el0,x17

	ldr x17,[x16,#8*33]
	msr spsr_el1,x17

	ldr x17,[x16,#8*34]//pc
	msr elr_el1,x17
	//add	sp, sp, 280		
	eret

