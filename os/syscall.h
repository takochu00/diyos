#ifndef _SYSCALL_H_
#define _SYSCALL_H_
#include <typedefs.h>
#include <interrupt.h>

typedef enum {
    DIY_SYSCALL_TYPE_RUN = 0,
    DIY_SYSCALL_TYPE_EXIT,
    DIY_SYSCALL_TYPE_WAIT,
    DIY_SYSCALL_TYPE_SLEEP,
    DIY_SYSCALL_TYPE_WAKEUP,
    DIY_SYSCALL_TYPE_GETID,
    DIY_SYSCALL_TYPE_CHANGE_PRIORITY,
    DIY_SYSCALL_TYPE_KMALLOC,
    DIY_SYSCALL_TYPE_KMFREE,
    DIY_SYSCALL_TYPE_MSGSEND,
    DIY_SYSCALL_TYPE_MSGRECV,
    DIY_SYSCALL_TYPE_SETINTR,
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
        struct {
            int size;
            void *ret;
        } kmalloc;
        struct {
            void *p;
            int ret;
        } kmfree;
        struct {
            diy_msgbox_id_t id;
            int size;
            char *p;
            int ret;
        } msgsend;
        struct {
            diy_msgbox_id_t id;
            int *psize;
            char **pp;
            diy_thread_id_t ret;
        } msgrecv;
        struct {
            softvec_type_t type;
            diy_handler_t handler;
            int ret;
        } setintr;
    } un;
} diy_syscall_param_t;

#endif//_SYSCALL_H_
