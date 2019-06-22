#include<entry.h>
#include<os_printf.h>
extern void sched_yield();
extern void put32(long,long);
extern void timer_init();
void enable_interrupt_controller()
{
	// enable all kind of exception!
	//as for the register mapping,see guidebook. 
	put32((0x40000040), (0xf));
    put32((0x40000044), (0xf));
    put32((0x40000048), (0xf));
    put32((0x4000004c), (0xf));
}
void handle_irq( void ) 
{
	//must reinitialize the timer
	//printf("Timer interrupt received\n\r");
	timer_init();
	
	sched_yield();
}
