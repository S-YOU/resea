#ifndef __KFS_H__
#define __KFS_H__

#include <resea/types.h>

struct kfs_dir {
    struct tar_ *current;
};

struct kfs_file {
    struct tar_ *tarfile;
    const char *name;
    const void *data;
    size_t length;
};

void kfs_init(void);
void kfs_opendir(struct kfs_dir *dir);
void kfs_readdir(struct kfs_file *file);

#endif
