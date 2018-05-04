KFS_DIR := kernel/kfs
LIBS_DIR := libs

mkfiles := $(filter-out %/server.mk, $(MAKEFILE_LIST))
build_mk := $(word $(words $(mkfiles)), $(mkfiles))
server_dir := $(dir $(build_mk))

server_name := $(name)
executable := $(server_dir)server.elf
server_objs := $(foreach obj, $(objs), $(server_dir)$(obj))
server_libs := resea $(filter resea, $(libs))
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
build_dir := $(server_dir)build
server_objs += $(all_objs)
server_include_dirs += $(all_include_dirs)

server_c_objs := $(addprefix $(build_dir)/, \
	$(patsubst %.c, %.o, $(wildcard $(server_objs:.o=.c))))
server_s_objs := $(addprefix $(build_dir)/, \
	$(patsubst %.S, %.o, $(wildcard $(server_objs:.o=.S))))

$(executable): $(server_c_objs) $(server_s_objs)
	$(PROGRESS) LD $@
	$(LD) $(LDFLAGS) --script $(LIBS_DIR)/resea/arch/$(ARCH)/app.ld -o $@ $^

$(server_c_objs): $(build_dir)/%.o: %.c
	$(PROGRESS) CC $@
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(addprefix -I, $(server_include_dirs)) -c -o $@ $<

$(server_s_objs): $(build_dir)/%.o: $%.S
	$(PROGRESS) CC $@
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(addprefix -I, $(server_include_dirs)) -c -o $@ $<

# libs
$(build_dir)/%.o: %.c
	$(PROGRESS) CC $@
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(addprefix -I, $(server_include_dirs)) -c -o $@ $<

$(KFS_DIR)/servers/$(server_name): $(executable)
	$(PROGRESS) ADD $@
	mkdir -p $(dir $@)
	cp $< $@

# Add to kfs the executable.
ifeq ($(filter $(server_name),$(SERVERS)),$(server_name))
all_kfs_files += $(KFS_DIR)/servers/$(server_name)
endif

all_objs :=
all_libs :=
all_include_dirs :=
included_subdirs :=
