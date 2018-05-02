ARCH ?= x64
KERNEL_SERVERS ?=

server_dirs := $(filter-out servers/server.mk, $(wildcard servers/*))
ifeq ($(KERNEL_SERVERS),all)
override KERNEL_SERVERS := $(notdir $(server_dirs))
endif

.PHONY: default build clean run test
default: build

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
include $(foreach dir, $(server_dirs), $(dir)/build.mk)

# Load kernel rules.
include kernel/kernel.mk

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
LD = /usr/local/opt/llvm/bin/ld.lld
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
