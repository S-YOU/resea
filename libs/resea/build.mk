objs := arch/$(ARCH)/start.o
include_dirs := include arch/$(ARCH)

include libs/resea/arch/$(ARCH)/arch.mk

include $(COMMON_MK)
