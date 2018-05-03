KFS_DIR := kernel/kfs
LIBS_DIR := libs

mkfiles := $(filter-out %/server.mk, $(MAKEFILE_LIST))
build_mk := $(word $(words $(mkfiles)), $(mkfiles))
server_dir := $(dir $(build_mk))

server_name := $(name)
executable := $(server_dir)server.elf
server_objs := $(foreach obj, $(objs), $(server_dir)$(obj))
server_libs := resea $(filter-out resea, $(libs))
server_include_dirs := $(include_dirs)

ifeq ($(lang), rust)
libs := rust $(filter-out rust, $(libs))
endif

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
all_objs := $(addprefix $(build_dir)/, $(all_objs))
server_include_dirs += $(all_include_dirs)

ifeq ($(lang), rust)
all_objs += $(server_dir)$(server_name).o

$(server_dir)$(server_name).o:
	cd $(server_dir) && \
	RUST_TARGET_PATH=../../libs/rust/targets xargo rustc \
		--quiet --lib --target $(ARCH) \
		-- --emit obj=../../$@
endif

$(executable): $(server_objs) $(all_objs)
	$(PROGRESS) LD $@
	$(LD) $(LDFLAGS) --script $(LIBS_DIR)/resea/arch/$(ARCH)/app.ld -o $@ $^

$(server_objs): %.o: %.c
	$(PROGRESS) CC $@
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
ifeq ($(filter $(server_name),$(KERNEL_SERVERS)),$(server_name))
all_kfs_files += $(KFS_DIR)/servers/$(server_name)
endif

lang :=
all_objs :=
all_libs :=
all_include_dirs :=
included_subdirs :=