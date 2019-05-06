#include <typedefs.h>
#include <syscall.h>
#include <diyos.h>

diy_thread_id_t diy_thread_run(diy_func_t func, char *name, int priority, int stacksize, int argc, char *argv[]){
    diy_syscall_param_t param;
    param.un.run.func = func;
    param.un.run.name = name;
    param.un.run.priority = priority;
    param.un.run.stacksize = stacksize;
    param.un.run.argc = argc;
    param.un.run.argv = argv;
    diy_syscall(DIY_SYSCALL_TYPE_RUN, &param);
    return param.un.run.ret;
}

void diy_thread_exit(void){
    diy_syscall(DIY_SYSCALL_TYPE_EXIT, NULL);
}

int diy_thread_wait(void){
    diy_syscall_param_t param;
    diy_syscall(DIY_SYSCALL_TYPE_WAIT, &param);
    return param.un.wait.ret;
}

int diy_thread_sleep(void){
    diy_syscall_param_t param;
    diy_syscall(DIY_SYSCALL_TYPE_SLEEP, &param);
    return param.un.sleep.ret;
}

int diy_thread_wakeup(diy_thread_id_t id){
    diy_syscall_param_t param;
    param.un.wakeup.id = id;
    diy_syscall(DIY_SYSCALL_TYPE_WAKEUP, &param);
    return param.un.wakeup.ret;
}

diy_thread_id_t diy_thread_getid(void){
    diy_syscall_param_t param;
    diy_syscall(DIY_SYSCALL_TYPE_GETID, &param);
    return param.un.getid.ret;
}

int diy_thread_change_priority(int priority){
    diy_syscall_param_t param;
    param.un.change_priority.priority = priority;
    diy_syscall(DIY_SYSCALL_TYPE_CHANGE_PRIORITY, &param);
    return param.un.change_priority.ret;
}

void *diy_thread_kmalloc(int size){
    diy_syscall_param_t param;
    param.un.kmalloc.size = size;
    diy_syscall(DIY_SYSCALL_TYPE_KMALLOC, &param);
    return param.un.kmalloc.ret;
}

int diy_thread_kmfree(void *p){
    diy_syscall_param_t param;
    param.un.kmfree.p = p;
    diy_syscall(DIY_SYSCALL_TYPE_KMFREE, &param);
    return param.un.kmfree.ret;
}
