include_dirs := .
objs := startup.o init.o thread.o serial.o gdt.o idt.o tss.o paging.o \
	smp.o apic.o ioapic.o cpu.o handler.o irq.o exception.o pic.o switch.o \
	idle.o

BOCHS ?= bochs
override CFLAGS += -O2 -Wall -Wextra -g3 --target=x86_64
override CFLAGS += -ffreestanding -fno-builtin -nostdinc -nostdlib -mcmodel=large
override CFLAGS += -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-avx -mno-avx2
override LDFLAGS +=
QEMUFLAGS += -d cpu_reset,page -D qemu.log -nographic

.PHONY: bochs
run: arch/x64/disk.img
	qemu-system-x86_64 $(QEMUFLAGS) -hda $<

bochs: arch/x64/disk.img
	rm -f arch/x64/disk.img.lock
	$(BOCHS) -qf arch/x64/boot/bochsrc

test: arch/x64/disk.img
	(sleep 3; echo -e "\x01cq") | qemu-system-x86_64 $(QEMUFLAGS) -hda $<

arch/x64/boot/mbr.elf: arch/x64/boot/mbr.o arch/x64/boot/mbr.ld
	$(PROGRESS) LD $@
	$(LD) $(LDFLAGS) --script arch/x64/boot/mbr.ld -o $@ $<

arch/x64/boot/mbr.bin: arch/x64/boot/mbr.elf
	$(PROGRESS) OBJCOPY $@
	$(OBJCOPY) -Obinary $< $@

arch/x64/disk.img: arch/x64/boot/mbr.bin kernel/kernel.elf
	$(PROGRESS) DD $@.tmp
	$(DD) if=/dev/zero of=$@.tmp bs=1M count=64
	$(PROGRESS) OFORMAT $@.tmp
	mformat -i $@.tmp -F -t 120000 -h 2 -s 4 ::
	$(PROGRESS) MCOPY $@.tmp
	mcopy -i $@.tmp kernel/kernel.elf ::/kernel.elf
	$(PROGRESS) CAT $@
	cat arch/x64/boot/mbr.bin $@.tmp > $@

include $(COMMON_MK)
