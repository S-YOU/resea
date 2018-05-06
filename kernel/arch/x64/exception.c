#include <kernel/types.h>
#include <kernel/memory.h>
#include "asm.h"
#include "exception.h"

void x64_handle_exception(u8_t exception, u64_t error) {

    /* We're using IST0's stack. Context switches in an exception
       handling context are prohibited. */
    switch (exception) {
        case EXP_PAGE_FAULT: {
            uptr_t address = asm_read_cr2();
            bool write = (error >> 1) & 1;
            bool user = (error >> 2) & 1;
            bool rsvd = (error >> 3) & 1;
            bool exec = (error >> 4) & 1;

            if (rsvd) {
                BUG("page fault: RSVD bit violation");
            }

            INFO("x64: #PF at %p (err=%#x)", address, error);
            handle_page_fault(address, user, write, exec);
            break;
        }
        default:
            PANIC("Exception %d", exception);
    }
}
