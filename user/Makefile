all: 
	aarch64-elf-gcc user.c -nostdlib -c  -o user.o
	aarch64-elf-gcc user_asm.S -c -o user_asm.o
	aarch64-elf-ld -o bin.o -T user.lds user.o user_asm.o 
clean:
	rm *.o