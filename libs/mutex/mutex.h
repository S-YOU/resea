#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <resea/types.h>

#define MUTEX_UNLOCKED 0
#define MUTEX_LOCKED 1
typedef int mutex_t;

void mutex_init(mutex_t *lock, int init);
void mutex_lock(mutex_t *lock);
void mutex_unlock(mutex_t *lock);

#endif
