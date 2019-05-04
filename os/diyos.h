#ifndef _DIYOS_H_
#define _DIYOS_H_
#include <typedefs.h>
#include <syscall.h>

//for syscall
diy_thread_id_t diy_thread_run(diy_func_t func, char *name, int stacksize, int argc, char *argv[]);
void diy_thread_exit(void);

//for library
void diy_start(diy_func_t func, char *name, int stacksize, int argc, char *argv[]);
void diy_sysdown(void);//for fatal error
void diy_syscall(diy_syscall_type_t type, diy_syscall_param_t *param);

//for test.
int test08_1_main(int argc, char *argv[]);


#endif//_DIYOS_H_
