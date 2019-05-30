%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $*.o

.PHONY: all clean

all: uart_boot.o

clean:
	rm -rf *.o *~

include ../../include.mk
