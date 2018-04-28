#include <stdarg.h>
#include <resea/arch.h>
#include <resea/types.h>
#include "printf.h"

#define abs(x) ((x < 0)? -x : x)


static void print_str(const char *s) {

    for (int i=0; s[i] != '\0'; i++)
        arch_putchar(s[i]);
}


static void print_int(uintmax_t base, uintmax_t v, uintmax_t len,
                      bool sign, bool alt, bool pad, bool sep) {

    static const char *nchars = "0123456789abcdef";
    char buf[20];
    uintmax_t i;

    /*
     *  -567
     *  ^
     */
    if (sign && (int) v < 0) {
        arch_putchar('-');
        v = abs((int) v);
    }

    /*
     *  0x00001234
     *  ^^
     */
    if (alt && base == 16) {
        print_str("0x");
    }

    /*
     *  0x00001234
     *    ^^^^
     */
    if (pad) {
        uintmax_t order, _v;
        for (order=1, _v=(uintmax_t) v; _v /= (uintmax_t) base; order++);
        for (uintmax_t j=order; j < len*2; j++) {
            arch_putchar('0');
            if (sep && j == len+1)
                arch_putchar('_');
        }
    }

    /*
     *  0x00001234
     *        ^^^^
     */
    for (int j=0; j < (int) sizeof(buf); j++)
        buf[j] = '\0';
    i = sizeof(buf) - 2;

    do {
        uintmax_t index;
        index  = ((uintmax_t) v % (uintmax_t) base);
        v     /= (uintmax_t) base;
        buf[i--] = nchars[index];
        if (sep && i == len+2)
            buf[i--] = '_';
    } while(v != 0);

    i = 0;
    for (int j=0; buf[i] == '\0' && j < (int) sizeof(buf); j++, i++);
    print_str(&buf[i]);
}


void vprintf(const char *fmt, va_list vargs) {
    for (int i=0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            bool alt = false;
            bool pad = false;
            char specifier;
            uintmax_t len = sizeof(uintmax_t); // 1: char, 2: short, 4: unsigned, ...

            for (;;) {
                i++;
                if (fmt[i] == '#') {
                    alt = true;
                } else if (fmt[i] == '0') {
                    pad = true;
                } else if (fmt[i] == '\0') {
                    specifier = '%';
                    break;
                } else {
                    specifier = fmt[i];
                    break;
                }
            }

            switch(specifier) {
            case '%':
                arch_putchar('%');
                break;
            case 'd':
                print_int(10, va_arg(vargs, uintmax_t), len, true,  alt, pad, false);
                break;
            case 'u':
                print_int(10, va_arg(vargs, uintmax_t), len, false, alt, pad, false);
                break;
            case 'p':
                alt = true;
                pad = true;
                // fallthrough
            case 'x':
                print_int(16, va_arg(vargs, uintmax_t), len, false, alt, pad, false);
                break;
            case 'c':
                arch_putchar(va_arg(vargs, int));
                break;
            case 's':
                print_str(va_arg(vargs, char *));
                break;
            default:
                arch_putchar('%');
                arch_putchar(fmt[i]);
            }
        } else {
            arch_putchar(fmt[i]);
        }
    }
}


// Note that this could be invoked in interrupt contexts. Be careful with
// locks.
int printf(const char *fmt, ...) {
    va_list vargs;

    va_start(vargs, fmt);
    vprintf(fmt, vargs);
    va_end(vargs);

    // FIXME
    return -1;
}
