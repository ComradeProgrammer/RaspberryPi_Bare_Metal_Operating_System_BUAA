#include<os_printf.h>
#include<uart.h>
#include<mmu.h>
#include<env.h>
#include<pmap.h>
#include<helpfunct.h>
extern void sched_yield();
int sys_putchar(long no,long ch){
    
    uart_send_boot(ch);
    return 0;
}
unsigned long sys_getenvid(void)
{
	return curenv->env_id;
}
void sys_yield(void)
{
	
	struct Trapframe * src = (struct Trapframe *)(KERNEL_SP - sizeof(struct Trapframe));
	struct Trapframe * dst = (struct Trapframe *)(TIMESTACK - sizeof(struct Trapframe));
	bcopy((void *)src,(void *)dst,sizeof(struct Trapframe));
	sched_yield();
}
int sys_env_destroy(int sysno, unsigned long envid)
{
	int r;
	struct Env *e;

	if ((r = envid2env(envid, &e, 1)) < 0) {
		return r;
	}

	printf("[%08x] destroying %08x\n", curenv->env_id, e->env_id);
	env_destroy(e);
	return 0;
}
int sys_set_pgfault_handler(int sysno, unsigned long envid, unsigned long func, unsigned long xstacktop)
{
	// Your code here.
	struct Env *env;
	if(envid2env(envid,&env,PTE_V)<0){
		printf("Sorry,in sys_set_pgfault_handler we can't get env by envid.\n");
		return -E_INVAL;
	}
	env->env_pgfault_handler = func;
	env->env_xstacktop = xstacktop;
	return 0;
}
int sys_mem_alloc(int sysno, unsigned long envid, unsigned long va, unsigned long perm)
{
	// Your code here.
	struct Env *env;
	struct Page *ppage;
	int ret;
	ret = 0;
	// check whether permission is legal
	if(!(perm&PTE_V)){
		printf("sys_mem_alloc:permission denined\n");
		return -E_INVAL;
	}
	
	// try to alloc a page
	ret=page_alloc(&ppage);
	if(ret<0){
		printf("sys_mem_alloc:failed to alloc a page\n");
		return -E_NO_MEM;
	}
	//try to check and get the env_id;
	ret=envid2env(envid,&env,1);
	if(ret<0){
		printf("sys_mem_alloc:failed to get the target env\n");
		return -E_BAD_ENV;
	}
	//now insert
	ret=page_insert(env->env_pgdir,ppage,va,perm);
	if(ret<0){
		printf("sys_mem_alloc:page_insert failed");
		return -E_NO_MEM;
	}
	return 0;

}

int sys_mem_map(int sysno, unsigned long srcid, unsigned long srcva, unsigned long dstid, unsigned long dstva,unsigned long perm)
{
	int ret;
	unsigned long round_srcva, round_dstva;
	struct Env *srcenv;
	struct Env *dstenv;
	struct Page *ppage;
	unsigned long*ppte;

	ppage = NULL;
	ret = 0;
	round_srcva = ROUNDDOWN(srcva, BY2PG);
	round_dstva = ROUNDDOWN(dstva, BY2PG);

    //your code here
	// get corresponding env
	if(envid2env(srcid,&srcenv,1)<0){ 
		printf("sys_mem_map:srcenv doesn't exist\n");
		return -E_BAD_ENV;
	}
	if(envid2env(dstid,&dstenv,1)<0){
		printf("sys_mem_map:dstenv doesn't exist\n");
		return -E_BAD_ENV;
	}
	
	
	// perm is valid?
	if( !(perm&PTE_V) ){
		printf("sys_mem_map:permission denied\n");
		return -E_NO_MEM;
	}
	//try to get the page
	ppage=page_lookup(srcenv->env_pgdir,round_srcva,&ppte);
	if(ppage==NULL){
		printf("sys_mem_map:page of srcva is invalid\n");
		return -E_NO_MEM;
	}
	//try to insert the page
	ret=page_insert(dstenv->env_pgdir,ppage,round_dstva,perm);
	if(ret<0){
		printf("sys_mem_map:page_insert denied\n");
		return -E_NO_MEM;
	}
	return 0;
}
int sys_mem_unmap(int sysno, unsigned long envid, unsigned long va)
{
	// Your code here.
	int ret=0;
	struct Env *env;
	ret=envid2env(envid,&env,1);//=========================
	if(ret<0){
		printf("sys_mem_alloc:failed to get the target env\n");
		return -E_BAD_ENV;
	}
	page_remove(env->env_pgdir,va);
	return ret;
}
int sys_env_alloc(void)
//TODO: IMPLEMENT THIS FUNCTION WILL NOT BE USED TEMPORARILY 
{
    
	// Your code here.
	int r;
	struct Env *e;
	struct Page* pp;
	unsigned long* newpage;
	unsigned long currentsp;
	// try to alloc a env
	r=env_alloc(&e,curenv->env_id);
	if(r<0){
		printf("sys_env_alloc:no free env");
		return r;
	}
	e->env_status=ENV_NOT_RUNNABLE;
	bcopy(KERNEL_SP-sizeof(struct Trapframe),(void*)(&(e->env_tf)),sizeof(struct Trapframe));
	(e->env_tf).x[0]=0;
	e->env_pri=curenv->env_pri;
	//copy a stack?
	//I have to use a fake fork
	currentsp=ROUNDDOWN(e->env_tf.sp,BY2PG);
	for(;currentsp<USTACKTOP;currentsp+=BY2PG){
		r=page_alloc(&pp);
		if(r<0){
			printf("alloc for new stack when fork error\n");
			return r;
		}
		newpage=page2pa(pp);
		bcopy(currentsp,newpage,BY2PG);
		r=page_insert(e->env_pgdir,pp,currentsp,PTE_V);
	}
	
	e->env_status=ENV_RUNNABLE;


	return e->env_id;
}
int sys_set_env_status(int sysno, unsigned long envid, unsigned long status)
{
	// Your code here.
	struct Env *env;
	int r;
	extern int cur_sched;
	extern struct Env_list env_sched_list[2];
	//printf("setting status\n");
	if(status!=ENV_RUNNABLE && status!=ENV_NOT_RUNNABLE&&status!=ENV_FREE){
		printf("set_env_status:wrong status");
		return -E_INVAL;
	}
	r=envid2env(envid,&env,0);
	if(r<0){
		printf("set_status:env is invalid\n");
		return -E_BAD_ENV;
	}
	if((status==ENV_NOT_RUNNABLE||status==ENV_FREE)&&env->env_status!=status){
		LIST_REMOVE(env,env_sched_link);
	}
	else if(status==ENV_RUNNABLE){
		LIST_INSERT_HEAD(&(env_sched_list[cur_sched]),env,env_sched_link);
	}
	env->env_status=status;
	return 0;
	//	panic("sys_env_set_status not implemented");
}
int sys_set_trapframe(int sysno, unsigned long  envid, struct Trapframe *tf)
{
	/* int ret;
	struct Env* e;
	ret=envid2env(envid,&e,1);
	if(ret<0){
		return ret;
	}
	e->env_tf=*tf;
	return 0;*/
	panic("sys_set_trapframe not implemented");
	return -1;
}
void sys_panic(int sysno, char *msg)
{
	// no page_fault_mode -- we are trying to panic!
	/* panic("%s", TRUP(msg));*/
}
void sys_ipc_recv(int sysno, unsigned long dstva)
{
	//extern int cur_sched;
	//extern struct Env_list env_sched_list[2];
	extern int remaining_time;
	void* src=(void*)KERNEL_SP-sizeof(struct Trapframe);
	void* dst=(void*)TIMESTACK-sizeof(struct Trapframe);
	//printf("%x called recv\n",curenv->env_id);
	if(dstva>=UTOP){
		printf("ipc_recv:dstva is greater than UTOP");
		return;
	}
	curenv->env_status=ENV_NOT_RUNNABLE;
	curenv->env_ipc_recving=1;
	curenv->env_ipc_dstva=dstva;
	LIST_REMOVE(curenv,env_sched_link);
	remaining_time=0;
	bcopy(src,dst,sizeof(struct Trapframe));
	sched_yield();
	
}
int sys_ipc_can_send(int sysno, unsigned long envid, unsigned long value, unsigned long srcva,unsigned long perm)
{

	int r;
	struct Env *e;
	struct Page *p;
	unsigned long * ppte;
	extern int cur_sched;
	extern struct Env_list env_sched_list[2];	
	//try to get the destination env
    //printf("called!");
	r=envid2env(envid,&e,0);
	if(r<0){
		printf("ipc_send:dstenv is invalid\n");
		return -E_BAD_ENV;
	}
	// check whether target env is requesting ipc
	if(e->env_status!=ENV_NOT_RUNNABLE||!e->env_ipc_recving){
		return 	-E_IPC_NOT_RECV; 
	} 
	//check whether source & target virtual address is valid
	if(srcva>=UTOP){
		printf("ipc_send:virtual address greater than UTOP\n");
		return -E_NO_MEM;
	}
	//try to get the page which will be sent later 
	if(srcva!=0){
		p=page_lookup(curenv->env_pgdir,srcva,&ppte);
		if(p==NULL){
			printf("ipc_send:destinated  page not exist");
			return -E_NO_MEM;
		}
		r=page_insert(e->env_pgdir,p,e->env_ipc_dstva,perm);
		if(r<0){
			printf("ipc_send:page_insert failed\n");
			return -E_NO_MEM;
		}	
	}
    //printf("[debug] to %x\n",e->env_id);
	e->env_ipc_value=value;
	e->env_ipc_from=curenv->env_id;
	e->env_ipc_perm=perm;
	e->env_ipc_recving=0;
	e->env_status=ENV_RUNNABLE;
   // printf("[debug] from %x\n",e->env_ipc_from);
	LIST_INSERT_HEAD(&(env_sched_list[cur_sched]),e,env_sched_link);

	return 0;
}

void * const sys_call_table[] ={sys_putchar,
                                sys_getenvid,
                                sys_yield,
                                sys_env_destroy,
                                sys_set_pgfault_handler,
                                sys_mem_alloc,
                                sys_mem_map,
                                sys_mem_unmap,
                                sys_env_alloc,
                                sys_set_env_status,
                                sys_set_trapframe,
                                sys_panic,
                                sys_ipc_can_send,
                                sys_ipc_recv,
                                };