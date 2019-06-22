#ifndef _SYSCALL_LIB_H_
#define _SYSCALL_LIB_H_

#define __SYSCALL_BASE 9527
#define __NR_SYSCALLS 20

#define SYS_putchar ((__SYSCALL_BASE ) + (0 ) )
#define SYS_getenvid 		((__SYSCALL_BASE ) + (1 ) )
#define SYS_yield			((__SYSCALL_BASE ) + (2 ) )
#define SYS_env_destroy		((__SYSCALL_BASE ) + (3 ) )
#define SYS_set_pgfault_handler	((__SYSCALL_BASE ) + (4 ) )
#define SYS_mem_alloc		((__SYSCALL_BASE ) + (5 ) )
#define SYS_mem_map			((__SYSCALL_BASE ) + (6 ) )
#define SYS_mem_unmap		((__SYSCALL_BASE ) + (7 ) )
#define SYS_env_alloc		((__SYSCALL_BASE ) + (8 ) )
#define SYS_set_env_status	((__SYSCALL_BASE ) + (9 ) )
#define SYS_set_trapframe		((__SYSCALL_BASE ) + (10 ) )
#define SYS_panic			((__SYSCALL_BASE ) + (11 ) )
#define SYS_ipc_can_send		((__SYSCALL_BASE ) + (12 ) )
#define SYS_ipc_recv		((__SYSCALL_BASE ) + (13 ) )
#define SYS_cgetc			((__SYSCALL_BASE ) + (14 ) )

void invalid_syscall(long a0);
int syscall_putchar(char ch);
unsigned long syscall_getenvid(void);
void syscall_yield(void);
void syscall_env_destroy(unsigned long envid);
int syscall_set_pgfault_handler(unsigned long envid, unsigned long func,unsigned long xstacktop);
int syscall_mem_alloc(unsigned long envid, unsigned long va, unsigned long perm);
int syscall_mem_map(unsigned long srcid, unsigned long srcva, unsigned long dstid, unsigned long dstva, unsigned long perm);
int syscall_mem_unmap(unsigned long envid, unsigned long va);
int syscall_env_alloc(void);
int syscall_set_env_status(unsigned long envid, unsigned long status);
//int syscall_set_trapframe(unsigned long envid, struct Trapframe *tf);
void syscall_panic(char *msg);
int syscall_ipc_can_send(unsigned long envid, unsigned long value, unsigned long srcva, unsigned long perm);
void syscall_ipc_recv(unsigned long dstva);
int syscall_cgetc();
#endif