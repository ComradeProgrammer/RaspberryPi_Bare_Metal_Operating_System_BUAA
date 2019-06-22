#include<syscall_lib.h>
#include<os_printf.h>
#include<env.h>
extern long msyscall(unsigned long,unsigned long,unsigned long,unsigned long,unsigned long,unsigned long);
void invalid_syscall(long a0){
    printf("invalid syscall :%x\n",a0);
    panic("error:syscall");
}

int syscall_putchar(char ch){
    //printf("syscall:%c@%x\n",ch,&ch);
    return msyscall(SYS_putchar,(long)ch,0,0,0,0);
}


unsigned long
syscall_getenvid(void)
{
	return msyscall(SYS_getenvid,0,0,0,0,0);
}

void
syscall_yield(void)
{
	msyscall(SYS_yield,0,0,0,0,0);
}


void
syscall_env_destroy(unsigned long envid)
{
	msyscall(SYS_env_destroy,envid,0,0,0,0);
}
int
syscall_set_pgfault_handler(unsigned long envid, unsigned long func, unsigned long xstacktop)
{
	return msyscall(SYS_set_pgfault_handler,envid,func,xstacktop,0,0);
}

int
syscall_mem_alloc(unsigned long envid, unsigned long va, unsigned long perm)
{
	return msyscall(SYS_mem_alloc,envid,va,perm,0,0);
}

int
syscall_mem_map(unsigned long srcid, unsigned long srcva, unsigned long dstid, unsigned long dstva, unsigned long perm)
{
	return msyscall(SYS_mem_map,srcid,srcva,dstid,dstva,perm);
}

int
syscall_mem_unmap(unsigned long envid, unsigned long va)
{
	return msyscall(SYS_mem_unmap,envid,va,0,0,0);
}

int syscall_env_alloc(void)
{
	
	int a= msyscall(SYS_env_alloc,0,0,0,0,0);
	//writef("syscall_env_alloc(): return : %x\n",a);
//	writef("");
	return a;
}

int
syscall_set_env_status(unsigned long envid, unsigned long status)
{
	return msyscall(SYS_set_env_status,envid,status,0,0,0);
}

int
syscall_set_trapframe(unsigned long envid, struct Trapframe *tf)
{
	//TODO: IMPLEMENT
	return  0;
}
void
syscall_panic(char *msg)
{
	//TODO: IMPLEMENT
}

int
syscall_ipc_can_send(unsigned long envid, unsigned long value, unsigned long srcva, unsigned long perm)
{
	return msyscall(SYS_ipc_can_send, envid, value, srcva, perm, 0);
}

void
syscall_ipc_recv(unsigned long dstva)
{
	msyscall(SYS_ipc_recv, dstva, 0, 0, 0, 0);
}

int 
syscall_cgetc()
{
	return msyscall(SYS_cgetc,0,0,0,0,0);
}
