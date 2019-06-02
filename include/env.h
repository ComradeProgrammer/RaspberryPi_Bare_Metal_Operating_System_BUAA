/* See COPYRIGHT for copyright information. */

#ifndef _ENV_H_
#define _ENV_H_

#include<queue.h>
struct Env {
    long tmp[128]; 
	//struct Trapframe env_tf;        // Saved registers
	LIST_ENTRY(Env) env_link;       // Free list
	unsigned long env_id;                   // Unique environment identifier
	unsigned long env_parent_id;            // env_id of this env's parent
	unsigned long env_status;               // Status of the environment
	unsigned long *env_pgdir;                // Kernel virtual address of page dir
	unsigned long env_cr3;

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

#define NENV (1<<10)
#endif // !_ENV_H_