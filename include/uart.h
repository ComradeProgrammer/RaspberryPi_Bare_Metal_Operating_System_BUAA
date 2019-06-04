#ifndef _UATR_H_
#define _UART_H

extern void uart_init_boot();
extern void uart_send_boot(unsigned int c);
extern char uart_getc_boot();
#endif