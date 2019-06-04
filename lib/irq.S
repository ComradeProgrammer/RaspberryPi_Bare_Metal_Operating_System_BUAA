
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