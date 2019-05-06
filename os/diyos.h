#ifndef _DIYOS_H_
#define _DIYOS_H_
#include <typedefs.h>
#include <syscall.h>

//for syscall
diy_thread_id_t diy_thread_run(diy_func_t func, char *name, int priority, int stacksize, int argc, char *argv[]);
void diy_thread_exit(void);
int diy_thread_wait(void);
int diy_thread_sleep(void);
int diy_thread_wakeup(diy_thread_id_t id);
diy_thread_id_t diy_thread_getid(void);
int diy_thread_change_priority(int priority);
void *diy_thread_kmalloc(int size);
int diy_thread_kmfree(void *p);

//for library
void diy_start(diy_func_t func, char *name, int priority, int stacksize, int argc, char *argv[]);
void diy_sysdown(void);//for fatal error
void diy_syscall(diy_syscall_type_t type, diy_syscall_param_t *param);

#endif//_DIYOS_H_
