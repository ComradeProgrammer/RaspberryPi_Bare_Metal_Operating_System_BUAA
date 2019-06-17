#include"uart_inline.h"
extern void inline user_uart_send_boot(unsigned int c);
void user_main(){
    while(1){
        user_uart_send_boot('a');
    }
}