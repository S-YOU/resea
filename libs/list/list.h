#ifndef __LIST_H__
#define __LIST_H__

#include <resea/types.h>

struct list {
    struct list *next;
};

void list_init(struct list **list);
void list_append(struct list **list, void *e);
void list_remove(struct list **list, void *e);

#endif
