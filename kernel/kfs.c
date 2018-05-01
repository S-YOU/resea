#include "kfs.h"


void kfs_opendir(struct kfs_dir *dir) {

}


void kfs_readdir(struct kfs_file *file) {

}


void kfs_init(void) {
}


DEPRECATED void kfs_container(void) {
    /* In case you call this function. */
    return;

    __asm__ __volatile__(
        ".pushsection .data\n"
        ".incbin \"kfs.tar\"\n"
        ".popsection\n"
    );
}
