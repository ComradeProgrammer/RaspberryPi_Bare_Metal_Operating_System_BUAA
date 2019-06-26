#include<entry.h>

.macro CLI
	msr	daifset, #2//close interrupt
.endm

.macro STI 
	msr    daifclr, #2//open interrupt
.endm

.macro handler label
.align 7
b \label
.endm

.macro saveall 
	sub	sp, sp, #280
	stp	x0, x1, [sp, #16 * 0]
	stp	x2, x3, [sp, #16 * 1]
	stp	x4, x5, [sp, #16 * 2]
	stp	x6, x7, [sp, #16 * 3]
	stp	x8, x9, [sp, #16 * 4]
	stp	x10, x11, [sp, #16 * 5]
	stp	x12, x13, [sp, #16 * 6]
	stp	x14, x15, [sp, #16 * 7]
	stp	x16, x17, [sp, #16 * 8]
	stp	x18, x19, [sp, #16 * 9]
	stp	x20, x21, [sp, #16 * 10]
	stp	x22, x23, [sp, #16 * 11]
	stp	x24, x25, [sp, #16 * 12]
	stp	x26, x27, [sp, #16 * 13]
	stp	x28, x29, [sp, #16 * 14]
	str	x30, [sp, #16 * 15] 
	mrs x16,sp_el0
	str x16,[sp,#8*31]
	mrs x16,elr_el1
	str x16,[sp,#8*32]
	str x16,[sp,#8*34]//pc

	mrs x16,spsr_el1
	str x16,[sp,#8*33]


.endm

.macro restore
    ldp	x0, x1, [sp, #16 * 0]
	ldp	x2, x3, [sp, #16 * 1]
	ldp	x4, x5, [sp, #16 * 2]
	ldp	x6, x7, [sp, #16 * 3]
	ldp	x8, x9, [sp, #16 * 4]
	ldp	x10, x11, [sp, #16 * 5]
	ldp	x12, x13, [sp, #16 * 6]
	ldp	x14, x15, [sp, #16 * 7]
	ldp	x16, x17, [sp, #16 * 8]
	ldp	x18, x19, [sp, #16 * 9]
	ldp	x20, x21, [sp, #16 * 10]
	ldp	x22, x23, [sp, #16 * 11]
	ldp	x24, x25, [sp, #16 * 12]
	ldp	x26, x27, [sp, #16 * 13]
	ldp	x28, x29, [sp, #16 * 14]
	ldr	x30, [sp, #16 * 15] 
	
	ldr x16,[sp,#8*31]
	msr sp_el0,x16

	ldr x16,[sp,#8*33]
	msr spsr_el1,x16

	ldr x16,[sp,#8*34]//pc
	msr elr_el1,x16
	add	sp, sp, #280		
	//eret
.endm


.macro unimplement_entry type
CLI
saveall
mov x0,#\type
mrs x1,esr_el1
mrs x2,elr_el1
bl unimplement_handler
.endm
//exception vectors
.align 11
.global vectors

vectors:
    handler	sync_invalid_el1t			// Synchronous EL1t
	handler	irq_invalid_el1t			// IRQ EL1t
	handler	fiq_invalid_el1t			// FIQ EL1t
	handler	error_invalid_el1t			// Error EL1t

	handler	sync_invalid_el1h			// Synchronous EL1h
	handler	el1_irq					// IRQ EL1h
	handler	fiq_invalid_el1h			// FIQ EL1h
	handler	error_invalid_el1h			// Error EL1h

	handler	el0_sync		// Synchronous 64-bit EL0
	handler	el0_irq			// IRQ 64-bit EL0
	handler	fiq_invalid_el0_64			// FIQ 64-bit EL0
	handler	error_invalid_el0_64			// Error 64-bit EL0

	handler	sync_invalid_el0_32			// Synchronous 32-bit EL0
	handler	irq_invalid_el0_32			// IRQ 32-bit EL0
	handler	fiq_invalid_el0_32			// FIQ 32-bit EL0
	handler	error_invalid_el0_32			// Error 32-bit EL0

sync_invalid_el1t:
	unimplement_entry  SYNC_INVALID_EL1t

irq_invalid_el1t:
	unimplement_entry  IRQ_INVALID_EL1t

fiq_invalid_el1t:
	unimplement_entry  FIQ_INVALID_EL1t

error_invalid_el1t:
	unimplement_entry  ERROR_INVALID_EL1t

sync_invalid_el1h:
	unimplement_entry  SYNC_INVALID_EL1h

fiq_invalid_el1h:
	unimplement_entry  FIQ_INVALID_EL1h

error_invalid_el1h:
	unimplement_entry  ERROR_INVALID_EL1h

sync_invalid_el0_64:
	unimplement_entry  SYNC_INVALID_EL0_64


fiq_invalid_el0_64:
	unimplement_entry  FIQ_INVALID_EL0_64

error_invalid_el0_64:
	unimplement_entry  ERROR_INVALID_EL0_64

sync_invalid_el0_32:
	unimplement_entry  SYNC_INVALID_EL0_32

irq_invalid_el0_32:
	unimplement_entry  IRQ_INVALID_EL0_32

fiq_invalid_el0_32:
	unimplement_entry  FIQ_INVALID_EL0_32

error_invalid_el0_32:
	unimplement_entry  ERROR_INVALID_EL0_32

el1_irq:
el0_irq:
	CLI
	ldr x16,=0x52000000//TIMESTACK
	mov sp,x16
	saveall
	
	bl handle_irq
	restore 
	//STI
	eret 

el0_sync:
.extern sys_call_table
.extern invalid_syscall
	CLI
	ldr x16,=0x51fff000//kernel_sp
	mov sp,x16
	saveall
	
	ldr x17,=9527
	sub x0,x0,x17
	cmp  x0,#20//===========20 syscalls
	b.ge invalid_syscall

	adr x17,sys_call_table
	add x17,x17,x0,lsl #3
	ldr x17,[x17]
	blr x17

	//restore		
	//STI
	ldr	 x1, [sp, #8 * 1]
	ldp	x2, x3, [sp, #16 * 1]
	ldp	x4, x5, [sp, #16 * 2]
	ldp	x6, x7, [sp, #16 * 3]
	ldp	x8, x9, [sp, #16 * 4]
	ldp	x10, x11, [sp, #16 * 5]
	ldp	x12, x13, [sp, #16 * 6]
	ldp	x14, x15, [sp, #16 * 7]
	ldp	x16, x17, [sp, #16 * 8]
	ldp	x18, x19, [sp, #16 * 9]
	ldp	x20, x21, [sp, #16 * 10]
	ldp	x22, x23, [sp, #16 * 11]
	ldp	x24, x25, [sp, #16 * 12]
	ldp	x26, x27, [sp, #16 * 13]
	ldp	x28, x29, [sp, #16 * 14]
	ldr	x30, [sp, #16 * 15] 
	
	ldr x16,[sp,#8*31]
	msr sp_el0,x16

	ldr x16,[sp,#8*33]
	msr spsr_el1,x16

	ldr x16,[sp,#8*34]//pc
	msr elr_el1,x16
	add	sp, sp, #280
	eret 


.globl irq_vector_init
.extern vectors
irq_vector_init:
	adr	x0, vectors		// load VBAR_EL1 with virtual
	msr	vbar_el1, x0		// vector table address
	ret
