INCLUDES := -I./ -I../ -I../include/

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

.PHONY: clean

all: syscall_lib.o msyscall.o ipc.o userfunct.o fork.o

clean:
	rm -rf *~ *.o

include ../include.mk