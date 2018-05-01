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


void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d;
    const uint8_t *s;

    d = (uint8_t *) dest;
    s = (const uint8_t *) src;

    for (size_t i=0; i < n; i++)
        d[i] = s[i];

    return dest;
}


int strcmp(const char *s1, const char *s2) {

    while (*s1 && *s2 && *s1 - *s2 == 0) {
        s1++;
        s2++;
    }

    return *s1 - *s2;
}


int strncmp(const char *s1, const char *s2, size_t n) {

    for (size_t i=0; *s1 && *s2 && *s1 - *s2 == 0 && i < n-1; i++) {
        s1++;
        s2++;
    }

    return *s1 - *s2;
}
