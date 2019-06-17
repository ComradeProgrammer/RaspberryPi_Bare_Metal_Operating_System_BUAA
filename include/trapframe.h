#ifndef _TRAPFRAME_H_
#define _TRAPFRAME_H_
struct Trapframe{
    unsigned long x[31];//register X0-30
    unsigned long sp;
    unsigned long elr_el1;//elr_el1 register,in mips we called epc
    unsigned long pstate;
    unsigned long pc;
};
#endif
