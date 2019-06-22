#include<os_printf.h>
#include<mmu.h>
#include<helpfunct.h>
#include<pmap.h>
#include<env.h>
/* These variables are set by mips_detect_memory() */
unsigned long maxpa;            /* Maximum physical address */
unsigned long npage;            /* Amount of memory(in pages) */
unsigned long basemem;          /* Amount of base memory(in bytes) */
unsigned long extmem;           /* Amount of extended memory(in bytes) */
struct Page *pages;

static struct Page_list page_free_list;	/* Free list of physical pages */
extern void tlb_invalidate();
void mips_detect_memory()
{
    /* Step 1: Initialize basemem.
     * (When use real computer, CMOS tells us how many kilobytes there are). */
	maxpa = 0x40000000;
	basemem = 1024*1024*1024;
    // Step 2: Calculate corresponding npage value.
	npage = basemem/(1024*4);
    extmem=0;
    printf("Physical memory: %dK available, ", (int)(maxpa / 1024));
    printf("base = %dK, extended = %dK\n", (int)(basemem / 1024),(int)(extmem / 1024));
}
static void *alloc(unsigned long n, unsigned long align, long clear)
{
    extern unsigned long freemem;
    unsigned long alloced_mem;
    freemem=ROUND(freemem,align);
    alloced_mem=freemem;
    freemem=freemem+n;
    if(clear){
        bzero((void*)alloced_mem,n);
    }
    if(freemem>=maxpa){
        printf("freemem:%x,maxpa:%x",freemem,maxpa);
        panic("out of memory");
    }
    return (void*)alloced_mem;
}
static unsigned long* boot_pgdir_walk(unsigned long* pud,unsigned long va,int create){
    unsigned long* pud_entryp;
    unsigned long *pmd,*pmd_entryp;
    unsigned long* pte,*pte_entryp;
    pud_entryp=pud+PUDX(va);
    pmd=(unsigned long*)(PTE_ADDR(*pud_entryp));
    if((((*pud_entryp)&(PTE_V))==0x0)){
        if(create){
            pmd= alloc(BY2PG,BY2PG,1);
            *pud_entryp=(unsigned long)pmd|TYPE_PAGE|PTE_AF|PTE_USER|PTE_ISH|PTE_NORMAL;
        }
    }
    pmd_entryp=pmd+PMDX(va);
    pte=(unsigned long*)(PTE_ADDR(*pmd_entryp));
    if((((*pmd_entryp)&(PTE_V))==0x0)){
        if(create){
            pte= alloc(BY2PG,BY2PG,1);
            *pmd_entryp=(unsigned long)pte|TYPE_PAGE|PTE_AF|PTE_USER|PTE_ISH|PTE_NORMAL;
        }
    }
    pte_entryp=pte+PTEX(va);
    return pte_entryp;
}
void boot_map_segment(unsigned long* pud,unsigned long va,unsigned long size,
    unsigned long pa,unsigned long perm){
        unsigned long i;
        unsigned long *pte_entry;
        if(size%BY2PG!=0){
            printf("the size is not multiple of BY2PG\n");
            return;
        }
        for(i=0;i<size;i+=BY2PG){
            pte_entry=boot_pgdir_walk(pud,va+i,1);
            *pte_entry=(pa+i)|perm|TYPE_PAGE|PTE_AF|PTE_USER|PTE_ISH|PTE_NORMAL;
        }
}
void aarch64_vm_init(){
    unsigned long n;
    extern unsigned long freemem;
    unsigned long*  timestack,*kernel_sp;
    printf("to memory %x for struct page directory.\n", _pg_dir);

    pages=(struct Page*)alloc(npage*sizeof(struct Page),BY2PG,1);
    printf("to memory %x for struct Pages.\n", freemem);
    n=ROUND(npage*sizeof(struct Page),BY2PG);
    //printf("npage:%x,size:%x\n",npage,n);
    boot_map_segment((unsigned long*)_pg_dir,UPAGES,n,(unsigned long)pages,PTE_V);

    envs=(struct Env*)alloc(NENV*sizeof(struct Env),BY2PG,1);
    printf("to memory %x for struct Envs.\n", freemem);
    n=ROUND(NENV*sizeof(struct Env),BY2PG);
    
    boot_map_segment((unsigned long*)_pg_dir,UENVS,n,(unsigned long)envs,PTE_V);
    //printf("nenvs:%x,size:%x\n",NENV,n);

    timestack=(unsigned long*)alloc(BY2PG,BY2PG,1);
    printf("to memory %x for TIMESTACK.\n", freemem);
    boot_map_segment((unsigned long*)_pg_dir,TIMESTACK-BY2PG,BY2PG,(unsigned long)timestack,PTE_V);

    kernel_sp=(unsigned long*)alloc(BY2PG,BY2PG,1);
    printf("to memory %x for KERNEL SP.\n", freemem);
    boot_map_segment((unsigned long*)_pg_dir,KERNEL_SP-BY2PG,BY2PG,(unsigned long)kernel_sp,PTE_V);

    printf("aarch64_vm_init:boot_pgdir is %x\n",_pg_dir);
    printf("pmap.c:\t ArmV8 vm init success\n");
}

void page_init(void){
    extern unsigned long freemem;
    LIST_INIT(&page_free_list);
    freemem = ROUND(freemem,BY2PG);

    long loop;
	long loopLength = freemem/BY2PG;
	for(loop=0;loop<loopLength;loop++){
		pages[loop].pp_ref = 1; 
	} 
    for(loop=loopLength;loop<npage;loop++){
		pages[loop].pp_ref = 0;
		LIST_INSERT_HEAD(&page_free_list,&pages[loop],pp_link);
	}
}

int page_alloc(struct Page** pp){
      struct Page *ppage_temp;
      if((ppage_temp=LIST_FIRST(&page_free_list))!=NULL){
		*pp = ppage_temp;//get and turn for the *pp
		LIST_REMOVE(ppage_temp,pp_link);//remove allocted page from free_list.
		/* Step 2: Initialize this page.* Hint: use `bzero`. */
		bzero((void *)page2pa(*pp),BY2PG);//init ok!
		return 0;
	}	
    return -E_NO_MEM;
}

void page_free(struct Page* pp){
    if(pp->pp_ref>0){
        return;
    }
    else if(pp->pp_ref==0){
        LIST_INSERT_HEAD(&page_free_list,pp,pp_link);
		return;
    }
    panic(":pp->pp_ref is less than zero\n");
}
int pgdir_walk(unsigned long *pud,unsigned long va,int create,unsigned long**ppte){
    struct Page* ppage;
    unsigned long* pud_entryp;
    unsigned long* pmd,*pmd_entryp;
    unsigned long* pte;
    int r;
    pud_entryp=pud+PUDX(va);
    pmd=(unsigned long*)(PTE_ADDR(*pud_entryp));
    if((((*pud_entryp)&(PTE_V))==0x0)&&create){
        r=page_alloc(&ppage);
        if(r!=0){
            *ppte=NULL;
            return -E_NO_MEM;
        }
        *pud_entryp=page2pa(ppage)|TYPE_PAGE|PTE_AF|PTE_USER|PTE_ISH|PTE_NORMAL;
        pmd=(unsigned long*)(page2pa(ppage));
        ppage->pp_ref++;
    }
    else if((((*pud_entryp)&(PTE_V))==0x0)){
        *ppte=NULL;
        return -E_NO_MEM;
    }
    pmd_entryp=pmd+PMDX(va);
    pte=(unsigned long*)(PTE_ADDR(*pmd_entryp));
    if((((*pmd_entryp)&(PTE_V))==0x0)&&create){
        r=page_alloc(&ppage);
        if(r!=0){
            *ppte=NULL;
            return -E_NO_MEM;
        }
        *pmd_entryp=page2pa(ppage)|TYPE_PAGE|PTE_AF|PTE_USER|PTE_ISH|PTE_NORMAL;
        pte=(unsigned long*)(page2pa(ppage));
        ppage->pp_ref++;
    }
     else if((((*pmd_entryp)&(PTE_V))==0x0)){
        *ppte=NULL;
        return -E_NO_MEM;
    }
    *ppte=(unsigned long*)(pte+PTEX(va));
    return 0;
}

int page_insert(unsigned long* pud,struct Page*pp,unsigned long va,unsigned int perm){
    int status;
    unsigned int PERM;
    PERM=perm|PTE_V;
    unsigned long* entry;
    pgdir_walk(pud,va,0,&entry);
    if(entry!=0&&(((*entry)&PTE_V)!=0)){
        if(pa2page(*entry)!=pp){
            
            page_remove(pud,va);
        }
        else{
            tlb_invalidate();
            *entry=(page2pa(pp)|PERM|TYPE_PAGE|PTE_AF|PTE_USER|PTE_ISH|PTE_NORMAL);
            return 0;
        }
    }
    tlb_invalidate();
    status=pgdir_walk(pud,va,1,&entry);
    if(status!=0){
        return -E_NO_MEM;
    }
    *entry=(page2pa(pp)|PERM|TYPE_PAGE|PTE_AF|PTE_USER|PTE_ISH|PTE_NORMAL);
    pp->pp_ref++;
    return 0;
}
struct Page* page_lookup(unsigned long* pud,unsigned long va,unsigned long **ppte ){
    struct Page* ppage;
    unsigned long* pte;
    pgdir_walk(pud,va,0,&pte);
    if(pte==0){
        return 0;
    }
    if((*pte & PTE_V)==0){
        return 0;
    }
    ppage=pa2page(*pte);
    if(ppte){
        *ppte=pte;
    }
    return ppage;
}
void page_decref(struct Page *pp) {
    if(--pp->pp_ref == 0) {
        page_free(pp);
    }
}
void page_remove(unsigned long* pud,unsigned long va){
    unsigned long* entry;
    struct Page *ppage;
    ppage=page_lookup(pud,va,&entry);
    if(ppage==0){
        return;
    }
    ppage->pp_ref--;
    if(ppage->pp_ref==0){
        page_free(ppage);
    }
    *entry=0;
    tlb_invalidate();
    return;
}
void page_check(){
    struct Page* pp,*pp0,*pp1,*pp2;
    struct Page_list fl;
    unsigned long* tmp;
    pp0 = pp1 = pp2 = 0;
    assert(page_alloc(&pp0) == 0);
    assert(page_alloc(&pp1) == 0);
    assert(page_alloc(&pp2) == 0);

    assert(pp0);
    assert(pp1 && pp1 != pp0);
    assert(pp2 && pp2 != pp1 && pp2 != pp0);

    fl = page_free_list;

    LIST_INIT(&page_free_list);

    assert(page_alloc(&pp) == -E_NO_MEM);
    page_free(pp0);
    page_free(pp1);
    assert(page_insert((unsigned long*)_pg_dir, pp2, 0x70000000, 0) ==0);
    assert(PTE_ADDR(((unsigned long*)_pg_dir)[PUDX(0x70000000)]) == page2pa(pp1));
    assert(pp1->pp_ref == 1);

    assert(page_insert((unsigned long*)_pg_dir, pp2, 0x70000000+BY2PG, 0) == 0);
    assert(pgdir_walk((unsigned long*)_pg_dir,0x70000000+BY2PG,0,&tmp)==0);
    assert(PTE_ADDR(*tmp)==page2pa(pp2));
    assert(pp2->pp_ref == 2);

    assert(page_alloc(&pp) == -E_NO_MEM);

    assert(page_insert((unsigned long*)_pg_dir, pp2, 0x70000000+BY2PG, 0) == 0);
    assert(page_alloc(&pp) == -E_NO_MEM);

    assert(page_insert((unsigned long*)_pg_dir, pp0, 0x71000000, 0) < 0);

    assert(page_insert((unsigned long*)_pg_dir, pp1, 0x70000000+BY2PG, 0) == 0);
    page_free_list=fl;
    
}