#include<syscall_lib.h>
#include<printf.h>

void invalid_syscall(long a0){
    printf("invalid syscall :%x\n",a0);
    panic("error:syscall");
}

int syscall_putchar(char ch){
    //printf("syscall:%c@%x\n",ch,&ch);
    msyscall(SYS_putchar,(long)ch,0,0,0,0);
}
