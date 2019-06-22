#ifndef _PMAP_H_
#define _PMAP_H_
#include<mmu.h>
#include<queue.h>
#include<os_printf.h>


LIST_HEAD(Page_list, Page);
typedef LIST_ENTRY(Page) Page_LIST_entry_t;
extern struct Page *pages;
extern unsigned long npage;
struct Page {
	Page_LIST_entry_t pp_link;	/* free list link */

	// Ref is the count of pointers (usually in page table entries)
	// to this page.  This only holds for pages allocated using 
	// page_alloc.  Pages allocated at boot time using pmap.c's "alloc"
	// do not have valid reference count fields.

	unsigned short pp_ref;
};

static inline unsigned long page2ppn(struct Page*pp){
    return pp-pages;
}
static inline unsigned long page2pa(struct Page *pp){
    return page2ppn(pp)<<12;
} 

static inline struct Page *pa2page(unsigned long pa)
{
	if (PPN(pa) >= npage)
		panic("pa2page called with invalid pa: %x", pa);
	return &pages[PPN(pa)];
}

extern struct Page *pages;


void mips_detect_memory();
//static void *alloc(unsigned long n, unsigned long align, long clear);
//static unsigned long* boot_pgdir_walk(unsigned long* pud,unsigned long va,int create);
void boot_map_segment(unsigned long* pgdir,unsigned long va,unsigned long size,
    unsigned long pa,unsigned long perm);
void aarch64_vm_init();
void page_init(void);
int page_alloc(struct Page** pp);
void page_free(struct Page* pp);
int pgdir_walk(unsigned long *pud,unsigned long va,int create,unsigned long**ppte);
int page_insert(unsigned long* pud,struct Page*pp,unsigned long va,unsigned int perm);
struct Page* page_lookup(unsigned long* pud,unsigned long va,unsigned long **ppte );
void page_decref(struct Page *pp) ;
void page_remove(unsigned long* pud,unsigned long va);
void page_check();
#endif /* _PMAP_H_ */