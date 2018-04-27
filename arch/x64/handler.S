.intel_syntax noprefix

.macro SAVE_REGS
  push  r15
  push  r14
  push  r13
  push  r12
  push  r11
  push  r10
  push  r9
  push  r8
  push  rbp
  push  rdi
  push  rsi
  push  rdx
  push  rcx
  push  rbx
  push  rax
.endm

.macro RESTORE_REGS
  pop   rax
  pop   rbx
  pop   rcx
  pop   rdx
  pop   rsi
  pop   rdi
  pop   rbp
  pop   r8
  pop   r9
  pop   r10
  pop   r11
  pop   r12
  pop   r13
  pop   r14
  pop   r15
.endm

.extern x64_handle_irq
.globl x64_irq_handler
x64_irq_handler:
    cli
    SAVE_REGS

    call x64_handle_irq

    RESTORE_REGS
    iretq

.extern x64_handle_exception
.macro EXP_HANDLER_WITH_ERRCODE num
.globl  x64_exp_handler\num
x64_exp_handler\num:
    cli
    /* Get error code from the stack and overwrite by SAVE_REGS. */
    push rax
    mov  rax, [rsp + 8]
    mov  [rsp - 112], rax
    pop  rax
    add  rsp, 8

    SAVE_REGS

    mov  rsi, [rsp + 120] /* error code */
    mov  rdi, \num
    call x64_handle_exception

    RESTORE_REGS
    iretq
.endm


.macro EXP_HANDLER_WITHOUT_ERRCODE num
.globl x64_exp_handler\num
x64_exp_handler\num:
    cli
    SAVE_REGS

    mov  rdi, \num
    call x64_handle_exception

    RESTORE_REGS
    iretq
.endm


EXP_HANDLER_WITHOUT_ERRCODE 0
EXP_HANDLER_WITHOUT_ERRCODE 1
EXP_HANDLER_WITHOUT_ERRCODE 2
EXP_HANDLER_WITHOUT_ERRCODE 3
EXP_HANDLER_WITHOUT_ERRCODE 4
EXP_HANDLER_WITHOUT_ERRCODE 5
EXP_HANDLER_WITHOUT_ERRCODE 6
EXP_HANDLER_WITHOUT_ERRCODE 7
EXP_HANDLER_WITH_ERRCODE    8
EXP_HANDLER_WITHOUT_ERRCODE 9
EXP_HANDLER_WITH_ERRCODE    10
EXP_HANDLER_WITH_ERRCODE    11
EXP_HANDLER_WITH_ERRCODE    12
EXP_HANDLER_WITH_ERRCODE    13
EXP_HANDLER_WITH_ERRCODE    14
EXP_HANDLER_WITH_ERRCODE    15
EXP_HANDLER_WITHOUT_ERRCODE 16
EXP_HANDLER_WITH_ERRCODE    17
EXP_HANDLER_WITHOUT_ERRCODE 18
EXP_HANDLER_WITHOUT_ERRCODE 19
EXP_HANDLER_WITHOUT_ERRCODE 20
