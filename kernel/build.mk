objs := init.o memory.o process.o thread.o ipc.o kfs.o elf.o
libs := printf list mutex string

include $(COMMON_MK)
