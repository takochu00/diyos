#include <typedefs.h>

extern void start(void);        /* startup */
extern void intr_softerr(void); //softerr intrrupt in intr.S
extern void intr_syscall(void); //syscall intrrupt in intr.S
extern void intr_serintr(void); //setintr intrrupt in intr.S

/*
 * setting interrupt vector.
 * start is set at the top of arrays by linker script.
 */
void (*vectors[])(void) = {
    start, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    intr_syscall, intr_softerr, intr_softerr, intr_softerr,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    intr_serintr, intr_serintr, intr_serintr, intr_serintr,
    intr_serintr, intr_serintr, intr_serintr, intr_serintr,
    intr_serintr, intr_serintr, intr_serintr, intr_serintr,
};
