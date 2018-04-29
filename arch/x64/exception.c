#include <resea/types.h>
#include <printf.h>
#include "asm.h"
#include "exception.h"

void x64_handle_exception(uint8_t exception, uint64_t error) {

    switch (exception) {
        case 14:
            int  operation = PAGE_WRITE;
            bool is_user = (error >> 2) & 1;
            bool is_invalid = error & 1;
            uintptr_t address = asm_read_cr3();
            page_fault_handler(operation, address);
            break;
    }

    PANIC("Exception %d", exception);
}
