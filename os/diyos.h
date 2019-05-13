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
int diy_thread_msgsend(diy_msgbox_id_t id, int size, char *p);
diy_thread_id_t diy_thread_msgrecv(diy_msgbox_id_t id, int *psize, char **pp);
int diy_thread_setintr(softvec_type_t type, diy_handler_t handler);

//for srvcall
int diy_service_wakeup(diy_thread_id_t id);
void *diy_service_kmalloc(int size);
int diy_service_kmfree(void *p);
int diy_service_msgsend(diy_msgbox_id_t id, int size, char *p);

//for library
void diy_start(diy_func_t func, char *name, int priority, int stacksize, int argc, char *argv[]);
void diy_sysdown(void);//for fatal error
void diy_syscall(diy_syscall_type_t type, diy_syscall_param_t *param);
void diy_srvcall(diy_syscall_type_t type, diy_syscall_param_t *param);

//system task
int console_driver_main(int argc, char *argv[]);
//user task
int command_main(int argc, char *argv[]);

#endif//_DIYOS_H_
