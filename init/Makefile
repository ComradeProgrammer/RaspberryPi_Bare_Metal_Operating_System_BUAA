INCLUDES := -I./ -I../ -I../include/

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

.PHONY: clean

all: main.o el.o printel.o mmu.o enablemmu.o 

clean:
	rm -rf *~ *.o

include ../include.mk
