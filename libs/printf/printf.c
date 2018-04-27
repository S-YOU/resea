#include <resea/arch.h>
#include "printf.h"

// Don't use mutex here! This could be called in an interrupt context.
int printf(const char *fmt, ...) {
    int n = 0;
    while (*fmt != '\0') {
        if (*fmt == '%') {
            fmt++;
            if (*fmt == '\0') {
                arch_putchar('%');
            }
        }

        arch_putchar(*fmt);
        fmt++;
        n++;
    }

    return n;
}
