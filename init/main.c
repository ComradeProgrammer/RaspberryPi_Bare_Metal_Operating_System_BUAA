#include <printf.h>

extern void uart_init_boot();
extern void uart_send_boot(unsigned int c);
extern char uart_getc_boot();
extern void printel();
extern void enable_mmu();
extern void  boot_mmu_setup();
void main() {
    printf("===main.c:\tmain is start ...===\n");
    printel();
    uart_init_boot();
    printf("Uart is initialized\n");
    boot_mmu_setup();
    enable_mmu();
    printf("MMU enabled\n");
    
  
    // arm64_init();
    printf("HALT! ECHOING EVERYTHING BACK\n");
    while (1) {
        uart_send_boot(uart_getc_boot());
    }
    
    panic("main is over is error!");
}
