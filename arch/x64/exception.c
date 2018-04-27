#include <resea/types.h>
#include <printf.h>
#include "exception.h"

void x64_handle_exception(uint8_t exception) {
    printf("Exception %d\n", exception);
}
