#ifndef _mmu_h
#define _mmu_h

extern char _end[]; 
extern char _start[];
extern char _pg_dir[]; 
extern volatile unsigned char _data[];
//memory class
#define PTE_NORMAL (0<<2)
#define PTE_DEVICE (1 << 2)
#define PTE_NON_CACHE (2 << 2)

#define TCR_T0SZ			(64 - 48) 
#define TCR_T1SZ			((64 - 48) << 16)
#define TCR_TG0_4K			(0 << 14)
#define TCR_TG1_4K			(2 << 30)
#define TCR_VALUE			(TCR_T0SZ | TCR_T1SZ | TCR_TG0_4K | TCR_TG1_4K)

#define BY2PG 4096
#define MEM_SIZE 0x40000000

#define TYPE_PAGE 0x3
#define TYPE_BLOCK 0x1
#define MM_ACCESS_PERMISSION	(0x01 << 6) 
#define PTE_V 0x1
#define PTE_AF			(0x1 << 10)
#define PTE_RO		(0x1 << 7)
#define PTE_USER (0x1<<6)

#define PTE_OSH      (2<<8)      // outter shareable
#define PTE_ISH      (3<<8)      // inner shareable

#define KERNEL_BASE 0xffff000000000000
/* Rounding; only works for n = power of two */
#define ROUND(a, n)	(((((unsigned long)(a))+(n)-1)) & ~((n)-1))
#define ROUNDDOWN(a, n)	(((unsigned long)(a)) & ~((n)-1))

#define PGDX(va) ((((unsigned long)(va))>>39)&0x1ff)
#define PUDX(va) ((((unsigned long)(va))>>30)&0x1ff)
#define PMDX(va) ((((unsigned long)(va))>>21)&0x1ff)
#define PTEX(va) ((((unsigned long)(va))>>12)&0x1ff)


#endif