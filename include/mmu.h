#ifndef _mmu_h
#define _mmu_h
#define NULL ((void*)0x0)
#define BY2PG 4096
#define MEM_SIZE (0x40000000)
#define KERNEL_BASE (0xffff000000000000)
//memory class
#define PTE_NORMAL (0<<2)
#define PTE_DEVICE (1 << 2)
#define PTE_NON_CACHE (2 << 2)

#define TYPE_PAGE 0x3
#define TYPE_BLOCK 0x1

#define PTE_V 0x1
#define PTE_AF			(0x1 << 10)//whether this page has been used, just set 1
#define PTE_RO		(0x1 << 7)//if seet,then read only
#define PTE_USER (0x1<<6) //if set,el0(user)can access,else,el0 can't access

#define PTE_OSH      (2<<8)      // outter shareable,use for device memory
#define PTE_ISH      (3<<8)      // inner shareable,use for normal memory


/* Rounding; only works for n = power of two */
#define ROUND(a, n)	(((((unsigned long)(a))+(n)-1)) & ~((n)-1))
#define ROUNDDOWN(a, n)	(((unsigned long)(a)) & ~((n)-1))

#define PUDX(va) ((((unsigned long)(va))>>30)&0x1ff)
#define PMDX(va) ((((unsigned long)(va))>>21)&0x1ff)
#define PTEX(va) ((((unsigned long)(va))>>12)&0x1ff)
#define PTE_ADDR(pte)	((unsigned long)(pte)&~0xFFF)
#define PPN(va)		(((unsigned long)(va))>>12)

#define E_BAD_ENV       2       // Environment doesn't exist or otherwise
				// cannot be used in requested action
#define E_INVAL		3	// Invalid parameter
#define E_NO_MEM	4	// Request failed due to memory shortage
#define E_NO_FREE_ENV   5       // Attempt to create a new environment beyond
				// the maximum allowed
#define E_IPC_NOT_RECV  6	// Attempt to send to env that is not recving.

// File system error codes -- only seen in user-level
#define	E_NO_DISK	7	// No free space left on disk
#define E_MAX_OPEN	8	// Too many files are open
#define E_NOT_FOUND	9 	// File or block not found
#define E_BAD_PATH	10	// Bad path
#define E_FILE_EXISTS	11	// File already exists
#define E_NOT_EXEC	12	// File not a valid executable

extern char _end[]; 
extern char _start[];
extern char _pg_dir[]; 
extern void enable_mmu();
extern void  boot_mmu_setup();
/* 
#define UPAGES 0x80000000
#define UENVS 0x81000000
#define TIMESTACK 0x82000000
#define KERNEL_SP (0x82000000-BY2PG)*/
#define UPAGES 0x50000000
#define UENVS 0x51000000
#define TIMESTACK 0x52000000
#define KERNEL_SP (0x52000000-BY2PG)
//now we need to configure the memory map(kernel)
/*
|	-----------------------------------0x82000000
|	TIMESTACK  			BY2PG
|	-----------------------------------0x82000000-BY2PG
|	KERNEL_SP 			BY2PG			
|	-----------------------------------0X82000000-2*BY2PG
|	struct envs
|	-----------------------------------0x81000000 UENVS
|	struct pages
|	------------------------------------0x80000000 UPAGES
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
_------------------------------------0x40000000+2M
|
|	DEVICE ADDR
|	---------------------------------0x3f000000
|
|	free physical page to alloc
|
D	-----------------------------------freemem
I
R	kernel page middle directory,page tables
E
C	--------------------------------
T	kernel page upper directory   4K
	----------------------------------_end,_pg_dir
M
A	kernel text
P
|	----------------------------------0x80000
|	kernel stack
|
_	----------------------------------0x0	
*/
#endif