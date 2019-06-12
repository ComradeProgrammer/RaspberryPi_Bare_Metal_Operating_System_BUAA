#include<uart.h>
void bcopy(const void *src, void *dst, unsigned long len)
{
	void *max;

	max = dst + len;
	// copy machine words while possible
	while (dst + 7 < max)
	{
		*(long *)dst = *(long *)src;
		dst+=8;
		src+=8;
	}
	// finish remaining 0-3 bytes
	while (dst < max)
	{
		*(char *)dst = *(char *)src;
		dst+=1;
		src+=1;
	}
}

void bzero(void *b, unsigned long len)
{
	void *max;

	max = b + len;

	//prlongf("init.c:\tzero from %x to %x\n",(long)b,(long)max);
	
	// zero machine words while possible

	while (b + 7 < max)
	{
		*(long *)b = 0;
		b+=8;
	}
	
	// finish remaining 0-3 bytes
	while (b < max)
	{
		*(char *)b++ = 0;
	}		
	
}
void user_maina(){
    while(1){
        uart_send_boot('1');
    }
}
void user_mainb(){
    while(1){
        uart_send_boot('2');
    }
}