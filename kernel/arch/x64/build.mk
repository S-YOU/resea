include_dirs := .
objs := startup.o init.o thread.o serial.o gdt.o idt.o tss.o paging.o \
	smp.o apic.o ioapic.o cpu.o handler.o irq.o exception.o pic.o switch.o \
	idle.o syscall.o

BOCHS ?= bochs
override CFLAGS += -O2 -Wall -Wextra -g3 --target=x86_64
override CFLAGS += -ffreestanding -fno-builtin -nostdinc -nostdlib -mcmodel=large
override CFLAGS += -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-avx -mno-avx2
override LDFLAGS +=
QEMUFLAGS += -d cpu_reset,page -D qemu.log -nographic

.PHONY: bochs
run: $(ARCH_DIR)/disk.img
	qemu-system-x86_64 $(QEMUFLAGS) -hda $<

bochs: $(ARCH_DIR)/disk.img
	rm -f $(ARCH_DIR)/disk.img.lock
	$(BOCHS) -qf $(ARCH_DIR)/boot/bochsrc

test: $(ARCH_DIR)/disk.img
	(sleep 3; echo -e "\x01cq") | qemu-system-x86_64 $(QEMUFLAGS) -hda $<

$(ARCH_DIR)/boot/mbr.elf: $(ARCH_DIR)/boot/mbr.o $(ARCH_DIR)/boot/mbr.ld
	$(PROGRESS) LD $@
	$(LD) $(LDFLAGS) --script $(ARCH_DIR)/boot/mbr.ld -o $@ $<

$(ARCH_DIR)/boot/mbr.bin: $(ARCH_DIR)/boot/mbr.elf
	$(PROGRESS) OBJCOPY $@
	$(OBJCOPY) -Obinary $< $@

$(ARCH_DIR)/disk.img: $(ARCH_DIR)/boot/mbr.bin kernel/kernel.elf
	$(PROGRESS) DD $@.tmp
	$(DD) if=/dev/zero of=$@.tmp bs=1M count=64
	$(PROGRESS) OFORMAT $@.tmp
	mformat -i $@.tmp -F -t 120000 -h 2 -s 4 ::
	$(PROGRESS) MCOPY $@.tmp
	mcopy -i $@.tmp kernel/kernel.elf ::/kernel.elf
	$(PROGRESS) CAT $@
	cat $(ARCH_DIR)/boot/mbr.bin $@.tmp > $@

include $(COMMON_MK)