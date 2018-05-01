objs := init.o memory.o process.o thread.o ipc.o kfs.o elf.o
libs := resea printf compiler list mutex string

include $(COMMON_MK)
