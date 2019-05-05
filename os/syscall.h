#ifndef _SYSCALL_H_
#define _SYSCALL_H_
#include <typedefs.h>

typedef enum {
    DIY_SYSCALL_TYPE_RUN = 0,
    DIY_SYSCALL_TYPE_EXIT,
    DIY_SYSCALL_TYPE_WAIT,
    DIY_SYSCALL_TYPE_SLEEP,
    DIY_SYSCALL_TYPE_WAKEUP,
    DIY_SYSCALL_TYPE_GETID,
    DIY_SYSCALL_TYPE_CHANGE_PRIORITY,
} diy_syscall_type_t;

typedef struct {
    union {
        struct {
            diy_func_t func;
            char *name;
            int priority;
            int stacksize;
            int argc;
            char **argv;
            diy_thread_id_t ret;
        } run;
        struct {
            int dummy;
        } exit;
        struct {
            int ret;
        } wait;
        struct {
            int ret;
        } sleep;
        struct {
            diy_thread_id_t id;
            int ret;
        } wakeup;
        struct {
            diy_thread_id_t ret;
        } getid;
        struct {
            int priority;
            int ret;
        } change_priority;
    } un;
} diy_syscall_param_t;

#endif//_SYSCALL_H_
