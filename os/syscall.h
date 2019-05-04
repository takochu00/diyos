#ifndef _SYSCALL_H_
#define _SYSCALL_H_
#include <typedefs.h>

typedef enum {
    DIY_SYSCALL_TYPE_RUN = 0,
    DIY_SYSCALL_TYPE_EXIT,
} diy_syscall_type_t;

typedef struct {
    union {
        struct {
            diy_func_t func;
            char *name;
            int stacksize;
            int argc;
            char **argv;
            diy_thread_id_t ret;
        } run;
        struct {
            int dummy;
        } exit;
    } un;
} diy_syscall_param_t;

#endif//_SYSCALL_H_
