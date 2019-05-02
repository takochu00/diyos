#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_
#include <typedefs.h>

//following is symbol defined at linker script
extern char softvec;
#define SOFTVEC_ADDR (&softvec)

typedef short softvec_type_t;
typedef void (*softvec_handler_t)(softvec_type_t type, uint32_t sp);
#define SOFTVECS ((softvec_handler_t *)SOFTVEC_ADDR)

#define INTR_ENABLE  asm volatile ("andc.b #0x3f,ccr")
#define INTR_DISABLE asm volatile ("orc.b #0xc0,ccr")

int softvec_init(void);
int softvec_setintr(softvec_type_t type, softvec_handler_t handler);
void interrupt(softvec_type_t type, uint32_t sp);

#endif//_INTERRUPT_H_
