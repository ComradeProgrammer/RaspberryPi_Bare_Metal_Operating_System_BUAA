#include<syscall_lib.h>
#include<os_printf.h>
extern int msyscall(long,long,long,long,long,long);
void invalid_syscall(long a0){
    printf("invalid syscall :%x\n",a0);
    panic("error:syscall");
}

int syscall_putchar(char ch){
    //printf("syscall:%c@%x\n",ch,&ch);
    return msyscall(SYS_putchar,(long)ch,0,0,0,0);
}
