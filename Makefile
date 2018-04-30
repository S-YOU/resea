ARCH ?= x64
COMMON_MK = $(shell pwd)/mk/common.mk

.PHONY: default build clean run test setup
default: build

ARCH_DIR = arch/$(ARCH)
override CFLAGS := $(CFLAGS) -Werror=implicit-function-declaration \
	-Werror=int-conversion -Werror=incompatible-pointer-types
override LDFLAGS := $(LDFLAGS)

all_objs :=
all_libs :=
all_include_dirs := .

include kernel/build.mk arch/$(ARCH)/build.mk
include $(foreach lib, $(all_libs), libs/$(lib)/build.mk)

# Set `y' to suppress annoying build messages.
V =
$(V).SILENT:
.SECONDARY:

CC = clang
LD = ld
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy
DD = dd

ifeq ($(shell uname), Darwin)
CC = /usr/local/opt/llvm/bin/clang
LD = sh -c 'exec -a ld.lld /usr/local/opt/llvm/bin/lld $$*'
DD = gdd
TOOLCHAIN_PREFIX = g
endif

PROGRESS ?= printf "  \033[1;35m%7s  \033[1;m%s\033[m\n"

build: kernel/kernel.elf

clean:
	rm -f */*.o */*/*.o */*/*/*.o \
		*/*.elf */*/*.elf */*/*/*.elf \
		*/*.img */*/*.img */*/*/*.img \
		*/*.tmp */*/*.tmp */*/*/*.tmp \
		*/*.bin */*/*.bin */*/*/*.bin

setup:
	brew install coreutils llvm binutils qemu mtools

kernel/kernel.elf: $(all_objs) $(ARCH_DIR)/kernel.ld
	$(PROGRESS) LD $@
	$(LD) $(LDFLAGS) --Map=kernel/kernel.map --script $(ARCH_DIR)/kernel.ld -o $@ $(all_objs)

%.o: %.S Makefile
	$(PROGRESS) CC $@
	$(CC) $(CFLAGS) -c -o $@ $<

%.deps: %.c Makefile
	$(PROGRESS) GENDEPS $@
	$(CC) $(CFLAGS) $(addprefix -I, $(all_include_dirs)) -MF $@ -MT $(<:.c=.o) -MM $<

%.o: %.c Makefile
	$(PROGRESS) CC $@
	$(CC) $(CFLAGS) $(addprefix -I, $(all_include_dirs)) -c -o $@ $<

-include $(all_objs:.o=.deps)
