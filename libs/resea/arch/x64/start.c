void main(void);

void __start(void) {
    __asm__ __volatile__(
        ".globl start; start:\n"
        "xchg %bx,%bx\n"
        "pushq $123\n"
    );
    main();
    for(;;);

    /* TODO: graceful termination */
    __asm__ __volatile__("int $3");
}
