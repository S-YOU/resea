mkfiles := $(filter-out %/server.mk, $(MAKEFILE_LIST))
build_mk := $(word $(words $(mkfiles)), $(mkfiles))
server_dir := $(dir $(build_mk))

server_objs := $(foreach obj, $(objs), $(server_dir)$(obj))
server_libs := $(libs)
server_include_dirs := $(include_dirs)

# Load libs.
objs :=
libs :=
include_dirs :=
all_objs :=
all_libs :=
all_include_dirs :=
included_subdirs :=

include $(foreach lib, $(server_libs), $(LIBS_DIR)/$(lib)/build.mk)
server_objs += $(all_objs)

all_objs :=
all_libs :=
all_include_dirs :=
included_subdirs :=

executable := $(server_dir)server.elf
$(executable): $(server_objs)
	$(PROGRESS) LD $@
	$(LD) $(LDFLAGS) --script $(LIBS_DIR)/resea/arch/$(ARCH)/server.ld -o $@ $^

$(KFS_DIR)/servers/$(name): $(executable)
	$(PROGRESS) ADD $@
	mkdir -p $(dir $@)
	cp $< $@

# Add to kfs the executable.
ifeq ($(filter $(name),$(SERVERS)),$(name))
all_kfs_files += $(KFS_DIR)/servers/$(name)
endif

server_objs :=
server_libs :=
server_include_dirs :=
