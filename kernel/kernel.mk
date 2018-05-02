KFS_DIR = kernel/kfs
LIBS_DIR = libs
ARCH_DIR = kernel/arch/$(ARCH)

include $(ARCH_DIR)/arch.mk

objs := init.o memory.o process.o thread.o ipc.o kfs.o elf.o
libs := printf list mutex string

kernel_objs := $(addprefix kernel/, $(objs)) $(addprefix $(ARCH_DIR)/, $(arch_objs))
kernel_libs := $(libs)

# Load libs.
all_objs :=
all_libs :=
all_include_dirs :=
included_subdirs :=
include $(foreach lib, $(kernel_libs), libs/$(lib)/build.mk)
kernel_objs += $(all_objs)
kernel_include_dirs := $(PWD) $(addprefix $(ARCH_DIR)/, $(arch_include_dirs)) \
	$(all_include_dirs)

kernel/kfs.o: kernel/kfs.bin
kernel/kfs.bin: $(all_kfs_files) tools/mkkfs
	$(PROGRESS) MKKFS $@
	./tools/mkkfs $@ $(KFS_DIR)

kernel/kernel.elf: $(kernel_objs) $(ARCH_DIR)/kernel.ld
	$(PROGRESS) "LD(K)" $@
	$(LD) $(LDFLAGS) --Map=kernel/kernel.map --script $(ARCH_DIR)/kernel.ld -o $@ $(kernel_objs)

%.o: %.S Makefile
	$(PROGRESS) "CC(K)" $@
	$(CC) $(CFLAGS) -c -o $@ $<

%.deps: %.c Makefile
	$(PROGRESS) "GENDEPS(K)" $@
	$(CC) $(CFLAGS) $(addprefix -I, $(kernel_include_dirs)) -MF $@ -MT $(<:.c=.o) -MM $<

%.o: %.c Makefile
	$(PROGRESS) "CC(K)" $@
	$(CC) $(CFLAGS) $(addprefix -I, $(kernel_include_dirs)) -c -o $@ $<

# Clear variables.
objs :=
libs :=
include_dirs :=
subdirs :=
included_subdirs :=

-include $(kernel_objs:.o=.deps)
