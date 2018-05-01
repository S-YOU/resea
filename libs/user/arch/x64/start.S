.intel_syntax noprefix

.extern main
.globl start
start:
    call main

    /* TODO: terminate garacefully */
    int 3
