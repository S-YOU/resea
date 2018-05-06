ARCH ?= x64
SERVERS ?=
BUILD_DIR ?= build

server_dirs := $(filter-out servers/server.mk, $(wildcard servers/*))
ifeq ($(SERVERS),all)
override SERVERS := $(notdir $(server_dirs))
endif

.PHONY: default build clean run test
default: build

override LDFLAGS := $(LDFLAGS)
override CFLAGS := $(CFLAGS) \
	-std=c11 \
    -Wall \
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
.SUFFIXES:

CC = clang
LD = ld
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy
STRIP = $(TOOLCHAIN_PREFIX)strip
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

build: $(BUILD_DIR)/kernel/kernel.elf

tools/genstub/parser/idlParser.py: tools/genstub/idl.g4
	$(PROGRESS) ANTLR4 $@
	antlr4 -Dlanguage=Python3 -o $(dir $@) $<
	touch $(dir $@)/__init__.py

clean:
	rm -rf $(BUILD_DIR)
