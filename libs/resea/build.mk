objs := arch/$(ARCH)/start.o
include_dirs := include arch/$(ARCH)
subdirs := arch/$(ARCH)

include libs/lib.mk
