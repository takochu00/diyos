#include <typedefs.h>
#include <syscall.h>
#include <diyos.h>

diy_thread_id_t diy_thread_run(diy_func_t func, char *name, int stacksize, int argc, char *argv[]){
    diy_syscall_param_t param;
    param.un.run.func = func;
    param.un.run.name = name;
    param.un.run.stacksize = stacksize;
    param.un.run.argc = argc;
    param.un.run.argv = argv;
    diy_syscall(DIY_SYSCALL_TYPE_RUN, &param);
    return param.un.run.ret;
}

void diy_thread_exit(void){
    diy_syscall(DIY_SYSCALL_TYPE_EXIT, NULL);
}
