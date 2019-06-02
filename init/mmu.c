#include<mmu.h>
#include<printf.h>

extern volatile unsigned char _data[];
unsigned long freemem;
void boot_mmu_setup(){
    unsigned long *pud,*pmd,*pmd_entry,*pte,*pte_entry;
    unsigned long i,r,t;
    freemem=(unsigned long)_pg_dir;
    freemem=ROUND(freemem,BY2PG);
    pud=(unsigned long*)freemem;// alloc pud
    freemem+=BY2PG;

    pud[PUDX(_end)]=(freemem|TYPE_PAGE|PTE_AF|PTE_USER|PTE_ISH|PTE_NORMAL );
    pmd=(unsigned long*)freemem;
    freemem+=BY2PG;
    //mapping all pages
    for(r=0;r<(0x3f000000>>21);r++){
        pmd_entry=&(pmd[r]);
        *pmd_entry=(freemem|TYPE_PAGE|PTE_AF|PTE_USER|PTE_ISH|PTE_NORMAL );
        freemem+=BY2PG;
        for(t=0;t<512;t++){
            i=(r<<21)+(t<<12);
            pte=(unsigned long*)((*pmd_entry)&(0xfffffffffffff000));
            pte_entry=&(pte[t]);
            
             if(i>=0x80000&&i<(unsigned long)(_data)){
                (*pte_entry)=i|TYPE_PAGE|PTE_AF|PTE_NORMAL|PTE_USER|PTE_RO;
                //it seems that all codesetions must be marked as read only
                //or the os can't run
             }
            else{
                (*pte_entry)=i|TYPE_PAGE|PTE_AF|PTE_NORMAL|PTE_USER;
            }
        }
    }
    for(r=(0x3f000000>>21);r<512;r++){
        pmd[r]=((r<<21)|TYPE_BLOCK|PTE_AF|PTE_USER|PTE_DEVICE);
    }
    printf(">>>Lower half page tables for EL1 generated!\n");
    printf(">>>test:current fremem:%x\n",freemem);
    return;
}


    






