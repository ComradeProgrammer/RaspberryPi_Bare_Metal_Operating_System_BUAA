INCLUDES := -I./ -I../ -I../include/
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

.PHONY: clean

all: print.o printf.o helpfunct.o env.o entry.o handleirq.o irq.o unimplement.o timer.o

clean:
	rm -rf *~ *.o

include ../include.mk
