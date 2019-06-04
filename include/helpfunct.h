#ifndef _help_funct_h
#define _help_funct_h

void bcopy(const void *src, void *dst, unsigned long len);
void bzero(void *b, unsigned long len);
#define assert(x)	\
	do {	if (!(x)) panic("assertion failed: %s", #x); } while (0)
extern unsigned long get_something();
#endif