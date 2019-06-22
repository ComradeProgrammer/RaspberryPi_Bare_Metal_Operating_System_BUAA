#include<mmu.h>
#include<env.h>
#include<pmap.h>
#include<os_printf.h>
#include<syscall_lib.h>

void
ipc_send(unsigned long whom, unsigned long val, unsigned long srcva, unsigned long perm)
{
	int r;
	while ((r=syscall_ipc_can_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV)
	{
		syscall_yield();
		//writef("QQ");
	}
	if(r == 0)
		return;
	panic("error in ipc_send: %d", r);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.  
//
// Hint: use env to discover the value and who sent it.
unsigned long
ipc_recv(unsigned long *whom, unsigned long dstva, unsigned long *perm)
{
//printf("ipc_recv:come 0\n");
	syscall_ipc_recv(dstva);
	
	if (whom)
		*whom = curenv->env_ipc_from;
	if (perm)
		*perm = curenv->env_ipc_perm;
	return curenv->env_ipc_value;
}