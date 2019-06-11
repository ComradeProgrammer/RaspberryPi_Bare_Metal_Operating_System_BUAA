#ifndef _ENV_H_
#define _ENV_H_
#include<trapframe.h>
#include<queue.h>
#define UTOP 0x7f400000
#define LOG2NENV	10
#define NENV (1<<10)
#define USTACKTOP (UTOP - 2*BY2PG)
#define UTEXT 0x00400000
#define ENVX(envid)	((envid) & (NENV - 1))

#define ENV_FREE	0
#define ENV_RUNNABLE		1
#define ENV_NOT_RUNNABLE	2

struct Env {
    struct Trapframe env_tf;         // Saved registers
	LIST_ENTRY(Env) env_link;       // Free list
	unsigned long env_id;                   // Unique environment identifier
	unsigned long env_parent_id;            // env_id of this env's parent
	unsigned long env_status;               // Status of the environment
	unsigned long *env_pgdir;   
	             // Kernel virtual address of page dir
	//unsigned long env_cr3;
	unsigned long env_pri;
	LIST_ENTRY(Env) env_sched_link;
	// Lab 4 IPC
	unsigned long env_ipc_value;            // data value sent to us 
	unsigned long env_ipc_from;             // envid of the sender  
	unsigned long env_ipc_recving;          // env is blocked receiving
	unsigned long env_ipc_dstva;		// va at which to map received page
	unsigned long env_ipc_perm;		// perm of page mapping received

	// Lab 4 fault handling
	unsigned long env_pgfault_handler;      // page fault state
	unsigned long env_xstacktop;            // top of exception stack

	// Lab 6 scheduler counts
	unsigned long env_runs;			// number of times been env_run'ed
};
extern struct Env *envs;
extern struct Env *curenv;	 

LIST_HEAD(Env_list, Env);
#endif // !_ENV_H_