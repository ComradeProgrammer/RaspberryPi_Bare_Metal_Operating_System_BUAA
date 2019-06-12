CROSS_COMPILE   := aarch64-elf-
CC              := $(CROSS_COMPILE)gcc
CFLAGS          := -Wall -ffreestanding -g 
LD              := $(CROSS_COMPILE)ld
