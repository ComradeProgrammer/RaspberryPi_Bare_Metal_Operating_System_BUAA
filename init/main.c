#include <os_printf.h>
#include<pmap.h>
#include<mmu.h>
#include<timerandirq.h>
#include<helpfunct.h>
#include<uart.h>
#include<trapframe.h>
#include<env.h>
extern void printel();
extern void irq_vector_init();
extern void enable_interrupt_controller();
extern void user_main_pingpong();
extern void user_main_fktest();
extern void user_main_fktest2();
void main() {
    //unsigned long* tmp;
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
    env_init();

    env_create_priority((unsigned char *)(&(user_main_fktest2)),0,1);
    //env_create_priority((unsigned char *)(&(user_main_pingpong)),0,1);

    irq_vector_init();
    //enable_irq();
    enable_interrupt_controller();
    timer_init();
    printf(">>>timer interrupt started!\n");  
    //========TEST===========================
   

    //========HALT==================================
    
    /*printf("HALT! ECHOING EVERYTHING BACK\n");
    while (1) {
        uart_send_boot(uart_getc_boot());
    }
    */
    printf("\n===>Моя маленькая операционная система<===\n");
    printf("===>За победу，вперед.Ура！<===\n\n");
    panic("main is over,Halt!");
}
