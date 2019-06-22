#ifndef _SYSCALL_LIB_H_
#define _SYSCALL_LIB_H_

#define __SYSCALL_BASE 9527
#define __NR_SYSCALLS 20

#define SYS_putchar ((__SYSCALL_BASE ) + (0 ) )

void invalid_syscall(long a0);
int syscall_putchar(char ch);
#endif