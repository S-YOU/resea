ARCH ?= x64
SERVERS ?=
COMMON_MK = $(shell pwd)/mk/common.mk
SERVER_MK = $(shell pwd)/mk/server.mk

.PHONY: default build clean run test
default: build

KFS_DIR = kernel/kfs
LIBS_DIR = libs
ARCH_DIR = kernel/arch/$(ARCH)
override LDFLAGS := $(LDFLAGS)
override CFLAGS := $(CFLAGS) \
    -Wall \
	-Wextra \
	-Werror=implicit-function-declaration \
	-Werror=int-conversion \
	-Werror=incompatible-pointer-types \
	-Werror=shift-count-overflow \
	-Werror=shadow

all_kfs_files :=

# Load server rules.
include $(foreach server, $(SERVERS), servers/$(server)/Makefile)

# Load kernel rules.
included_subdirs :=
all_objs :=
all_libs :=
all_include_dirs := .
include kernel/build.mk $(ARCH_DIR)/build.mk
include $(foreach lib, $(all_libs), libs/$(lib)/build.mk)

# Set `y' to suppress annoying build messages.
V =
$(V).SILENT:
.SECONDARY:

CC = clang
LD = ld
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy
DD = dd
TAR = tar

ifeq ($(shell uname), Darwin)
CC = /usr/local/opt/llvm/bin/clang
LD = sh -c 'exec -a ld.lld /usr/local/opt/llvm/bin/lld $$*'
DD = gdd
TAR = gtar
TOOLCHAIN_PREFIX = g
endif

PROGRESS ?= printf "  \033[1;35m%7s  \033[1;m%s\033[m\n"

build: kernel/kernel.elf

clean:
	rm -f */*.o */*/*.o */*/*/*.o \
		*/*.elf */*/*.elf */*/*/*.elf \
		*/*.img */*/*.img */*/*/*.img \
		*/*.tmp */*/*.tmp */*/*/*.tmp \
		*/*.bin */*/*.bin */*/*/*.bin \
		kernel/kfs.tar
	rm -rf kernel/kfs

kernel/kfs.o: kernel/kfs.bin
kernel/kfs.bin: $(all_kfs_files)
	$(PROGRESS) MKKFS $@
	./tools/mkkfs $@ $(KFS_DIR)

kernel/kernel.elf: $(all_objs) $(ARCH_DIR)/kernel.ld
	$(PROGRESS) "LD(K)" $@
	$(LD) $(LDFLAGS) --Map=kernel/kernel.map --script $(ARCH_DIR)/kernel.ld -o $@ $(all_objs)

%.o: %.S Makefile
	$(PROGRESS) "CC(K)" $@
	$(CC) $(CFLAGS) -c -o $@ $<

%.deps: %.c Makefile
	$(PROGRESS) "GENDEPS(K)" $@
	$(CC) $(CFLAGS) $(addprefix -I, $(all_include_dirs)) -MF $@ -MT $(<:.c=.o) -MM $<

%.o: %.c Makefile
	$(PROGRESS) "CC(K)" $@
	$(CC) $(CFLAGS) $(addprefix -I, $(all_include_dirs)) -c -o $@ $<

-include $(all_objs:.o=.deps)
