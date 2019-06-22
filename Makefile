drivers_dir   := drivers
boot_dir      := boot
init_dir      := init
lib_dir       := lib
mm_dir        := mm
vmlinux_elf   := kernel.elf
vmlinux_img   := kernel8.img
link_script   := kernel.lds

modules       := boot drivers init lib mm user
objects       := $(boot_dir)/*.o \
                 $(init_dir)/*.o \
                 $(drivers_dir)/rpi3/*.o \
                 $(lib_dir)/*.o \
				 $(mm_dir)/*.o \
				 user/*.o

.PHONY: all $(modules) clean debug run

all: $(modules) vmlinux

vmlinux: $(modules)
	$(LD) -o $(vmlinux_elf) -T$(link_script) $(objects)
	$(CROSS_COMPILE)objcopy $(vmlinux_elf) -O binary $(vmlinux_img)

$(modules):
	$(MAKE) --directory=$@

clean:
	for d in $(modules); \
		do \
			$(MAKE) --directory=$$d clean; \
		done; \
	rm -rf *.o *~ $(vmlinux_elf) $(vmlinux_img)
run:
	qemu-system-aarch64 -M raspi3 -serial stdio -kernel kernel8.img
debug:
	qemu-system-aarch64 -S -s -M raspi3 -serial stdio -kernel kernel8.img
include include.mk