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

int diy_thread_msgsend(diy_msgbox_id_t id, int size, char *p){
    diy_syscall_param_t param;
    param.un.msgsend.id = id;
    param.un.msgsend.size = size;
    param.un.msgsend.p = p;
    diy_syscall(DIY_SYSCALL_TYPE_MSGSEND, &param);
    return param.un.msgsend.ret;
}

diy_thread_id_t diy_thread_msgrecv(diy_msgbox_id_t id, int *psize, char **pp){
    diy_syscall_param_t param;
    param.un.msgrecv.id = id;
    param.un.msgrecv.psize = psize;
    param.un.msgrecv.pp = pp;
    diy_syscall(DIY_SYSCALL_TYPE_MSGRECV, &param);
    return param.un.msgrecv.ret;
}

int diy_thread_setintr(softvec_type_t type, diy_handler_t handler){
    diy_syscall_param_t param;
    param.un.setintr.type = type;
    param.un.setintr.handler = handler;
    diy_syscall(DIY_SYSCALL_TYPE_SETINTR, &param);
    return param.un.setintr.ret;
}

int diy_service_wakeup(diy_thread_id_t id){
    diy_syscall_param_t param;
    param.un.wakeup.id = id;
    diy_srvcall(DIY_SYSCALL_TYPE_WAKEUP, &param);
    return param.un.wakeup.ret;
}

void *diy_service_kmalloc(int size){
    diy_syscall_param_t param;
    param.un.kmalloc.size = size;
    diy_srvcall(DIY_SYSCALL_TYPE_KMALLOC, &param);
    return param.un.kmalloc.ret;
}

int diy_service_kmfree(void *p){
    diy_syscall_param_t param;
    param.un.kmfree.p = p;
    diy_srvcall(DIY_SYSCALL_TYPE_KMFREE, &param);
    return param.un.kmfree.ret;
}

int diy_service_msgsend(diy_msgbox_id_t id, int size, char *p){
    diy_syscall_param_t param;
    param.un.msgsend.id = id;
    param.un.msgsend.size = size;
    param.un.msgsend.p = p;
    diy_srvcall(DIY_SYSCALL_TYPE_MSGSEND, &param);
    return param.un.msgsend.ret;
}
