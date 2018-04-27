#include "string.h"

void *memset(void *p, int c, size_t n) {
    uint8_t *_p;

    _p = (uint8_t *) p;

    while(n > 0) {
        *_p = (uint8_t) c;
        n--;
        _p++;
    }

    return _p;
}
