#include <kernel/types.h>
#include <printf.h>
#include <kernel/memory.h>
#include "asm.h"
#include "exception.h"

void x64_handle_exception(uint8_t exception, uint64_t error) {

    /* We're using IST0's stack. Context switches in an exception
       handling context are prohibited. */
    switch (exception) {
        case EXP_PAGE_FAULT: {
            uintptr_t address = asm_read_cr2();
            bool invalid = (error >> 0) & 1;
            bool write = (error >> 1) & 1;
            bool user = (error >> 2) & 1;
            bool rsvd = (error >> 3) & 1;
            bool exec = (error >> 4) & 1;

            if (rsvd) {
                BUG("page fault: RSVD bit violation");
            }

            INFO("err: %p", error);
            handle_page_fault(address, invalid, user, write, exec);
            break;
        }
        default:
            PANIC("Exception %d", exception);
    }
}
