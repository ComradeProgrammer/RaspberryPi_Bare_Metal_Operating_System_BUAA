.global timer_init
timer_init:
    mov x0,#0x3
    msr cntkctl_el1,x0
    ldr x0,=(0x3b9aca0>>6)
    //when the qemu start,the frequency is 0x3b9aca0 HZ
    //i don't know why i cannot change the frquency of it
    //doing so to so set 1s per exception
    msr cntp_tval_el0,x0
    mov x0,#0x1
    msr cntp_ctl_el0,x0
    ret

.global get_something
get_something:
    mov x0,#0
    mrs x0,cntfrq_el0
    ret