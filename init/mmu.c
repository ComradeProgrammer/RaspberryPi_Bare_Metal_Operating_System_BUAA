
#include<mmu.h>
#include<printf.h>
void boot_mmu_setup(){
    unsigned long tmp=(unsigned long)_pg_dir;
    unsigned long *pud,*pmd,*pmd_entry,*pte,*pte_entry;
    unsigned long i,r,t;
    tmp=ROUND(tmp,BY2PG);
    pud=(unsigned long*)tmp;
    tmp+=BY2PG;
    
    pud[PUDX(_end)]=(tmp|TYPE_PAGE|PTE_AF|PTE_USER|PTE_ISH|PTE_NON_CACHE);
    pmd=(unsigned long*)tmp;
    tmp+=BY2PG;
    //mapping all pages
    for(r=0;r<(0x3f000000>>21);r++){
        pmd_entry=&(pmd[r]);
        *pmd_entry=(tmp|TYPE_PAGE|PTE_AF|PTE_USER|PTE_ISH|PTE_NON_CACHE);
        tmp+=BY2PG;
        for(t=0;t<512;t++){
            i=(r<<21)+(t<<12);
            pte=(unsigned long*)((*pmd_entry)&(0xfffffffffffff000));
            pte_entry=&(pte[t]);
            
             if(i>=0x80000&&i<(unsigned long)(_data)){
                (*pte_entry)=i|TYPE_PAGE|PTE_AF|PTE_NON_CACHE|PTE_USER|PTE_RO;
             }
            else{
                (*pte_entry)=i|TYPE_PAGE|PTE_AF|PTE_NON_CACHE|PTE_USER;
                
            }
        }
    }
    for(r=(0x3f000000>>21);r<512;r++){
        pmd[r]=((r<<21)|TYPE_BLOCK|PTE_AF|PTE_USER|PTE_DEVICE);
    }
    printf("Lower half page tables for EL1 generated!\n");
    return;
}


    






