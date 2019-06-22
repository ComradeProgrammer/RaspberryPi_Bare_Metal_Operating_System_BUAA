#include<os_printf.h>
#include<uart.h>
int sys_putchar(long no,long ch){
    
    uart_send_boot(ch);
    return 0;
}
void * const sys_call_table[] ={sys_putchar};