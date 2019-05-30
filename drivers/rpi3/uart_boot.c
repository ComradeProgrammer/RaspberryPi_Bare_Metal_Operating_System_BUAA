
void uart_init_boot() {
#define UART0_IBRD      ((volatile unsigned int*)(0x3F201024))
#define UART0_FBRD      ((volatile unsigned int*)(0x3F201028))
#define UART0_LCRH      ((volatile unsigned int*)(0x3F20102C))
#define UART0_CR        ((volatile unsigned int*)(0x3F201030))
#define UART0_ICR       ((volatile unsigned int*)(0x3F201044))
#define GPFSEL1         ((volatile unsigned int*)(0x3F200004))
#define GPPUD           ((volatile unsigned int*)(0x3F200094))
#define GPPUDCLK0       ((volatile unsigned int*)(0x3F200098))
#define UART0_DR        ((volatile unsigned int*)(0x3F201000))
#define UART0_FR        ((volatile unsigned int*)(0x3F201018))
    register unsigned int r;
    *UART0_CR = 0;
    r = *GPFSEL1;
    r &= ~((7 << 12) | (7 << 15)); // gpio14, gpio15
    r |= (4 << 12) | (4 << 15);    // alt0
    *GPFSEL1 = r;
    *GPPUD = 0;            // enable pins 14 and 15
    r = 150;
    while (r--) { asm volatile("nop"); }
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    r = 150;
    while (r--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;        // flush GPIO setup
    *UART0_ICR = 0x7FF;    // clear interrupts
    *UART0_IBRD = 2;       // 115200 baud
    *UART0_FBRD = 0xB;
    *UART0_LCRH = 0b11 << 5; // 8n1
    *UART0_CR = 0x301;     // enable Tx, Rx, FIFO
}

void uart_send_boot(unsigned int c) {
    do { asm volatile("nop"); } while (*UART0_FR & 0x20);
    *UART0_DR = c;
}


char uart_getc_boot() {
    char r;
    do { asm volatile("nop"); } while (*UART0_FR & 0x10);
    r = (char)(*UART0_DR);
    return r == '\r' ? '\n' : r;
}
