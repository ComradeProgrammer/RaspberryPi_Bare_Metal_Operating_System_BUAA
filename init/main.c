#include <printf.h>
#include<pmap.h>
#include<mmu.h>
#include<timerandirq.h>
#include<helpfunct.h>
#include<uart.h>
extern void printel();

void main() {
    unsigned long* tmp;
    printf(">>>main.c:\tmain is start ...>>>\n");
    printel();
    uart_init_boot();
    printf(">>>Uart is initialized\n");
    boot_mmu_setup();
    enable_mmu();
    printf(">>>MMU enabled\n");
    mips_detect_memory();
    aarch64_vm_init();
    page_init();
    irq_vector_init();
    enable_irq();
    enable_interrupt_controller();
    timer_init();
    printf(">>>timer interrupt started!\n");  
    //========TEST===========================
    env_init();
    env_check();
    //page_check();
    //========HALT==================================
    
    /*printf("HALT! ECHOING EVERYTHING BACK\n");
    while (1) {
        uart_send_boot(uart_getc_boot());
    }
    */
    panic("main is over ^^^^^^^^^^^^^^^^^^^^^^^");
}
