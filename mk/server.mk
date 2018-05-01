mkfiles := $(filter-out %/server.mk, $(MAKEFILE_LIST))
build_mk := $(word $(words $(mkfiles)), $(mkfiles))
server_dir := $(dir $(build_mk))

server_name := $(name)
server_objs := $(foreach obj, $(objs), $(server_dir)$(obj))
server_libs := $(libs)
server_include_dirs := $(include_dirs)

# Load libs.
name :=
objs :=
libs :=
include_dirs :=
all_objs :=
all_libs :=
all_include_dirs :=
included_subdirs :=

include $(foreach lib, $(server_libs), $(LIBS_DIR)/$(lib)/build.mk)
server_objs += $(all_objs)
server_include_dirs += $(all_include_dirs)


all_include_dirs :=
all_objs :=
all_libs :=
all_include_dirs :=
included_subdirs :=

executable := $(server_dir)server.elf
$(executable): $(server_objs)
	$(PROGRESS) LD $@
	$(LD) $(LDFLAGS) --script $(LIBS_DIR)/resea/arch/$(ARCH)/app.ld -o $@ $^

$(server_objs): %.o: %.c Makefile
	$(PROGRESS) CC $@
	$(CC) $(CFLAGS) $(addprefix -I, $(server_include_dirs)) -c -o $@ $<

$(KFS_DIR)/servers/$(server_name): $(executable)
	$(PROGRESS) ADD $@
	mkdir -p $(dir $@)
	cp $< $@

# Add to kfs the executable.
ifeq ($(filter $(server_name),$(SERVERS)),$(server_name))
all_kfs_files += $(KFS_DIR)/servers/$(server_name)
endif
