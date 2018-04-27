#include "mutex.h"


void mutex_init(mutex_t *lock, int init) {
    *lock = init;
}


void mutex_lock(mutex_t *lock) {
    while(!atomic_compare_and_swap(lock, MUTEX_UNLOCKED, MUTEX_LOCKED));
}


void mutex_unlock(mutex_t *lock) {
    *lock = MUTEX_UNLOCKED;
}
