#include <printf.h>
#include<pmap.h>
#include<mmu.h>
extern void uart_init_boot();
extern void uart_send_boot(unsigned int c);
extern char uart_getc_boot();
extern void printel();
extern void enable_mmu();
extern void  boot_mmu_setup();
void main() {
    unsigned long* tmp;
    printf(">>>main.c:\tmain is start ...>>>\n");
    printel();
    uart_init_boot();
    printf(">>>Uart is initialized\n");
    boot_mmu_setup();
    enable_mmu();
    printf(">>>MMU enabled\n");

    //========TEST===========================
   mips_detect_memory();
   /* tmp=boot_pgdir_walk(_pg_dir,0x80000000,1);
    printf("test:%x@%x\n",(*tmp),tmp);*/
    aarch64_vm_init();
    page_init();
    page_check();
    //==========================================
    
    printf("HALT! ECHOING EVERYTHING BACK\n");
    while (1) {
        uart_send_boot(uart_getc_boot());
    }
    
    panic("main is over is error!");
}
