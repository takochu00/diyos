#include <typedefs.h>
#include <diyos.h>
#include <intrdefs.h>
#include <interrupt.h>
#include <syscall.h>
#include <lib.h>

#define THREAD_NUM 6
#define THREAD_NAME_SIZE 15

typedef struct diy_thread_ctx {
    uint32_t sp;//stack pointer
} diy_thread_ctx;

typedef struct diy_thread {
    struct diy_thread *next;//for thread task ready queue.
    char name[THREAD_NAME_SIZE];
    char *stack;

    struct {
        diy_func_t func;
        int argc;
        char **argv;
    } init;

    struct {
        diy_syscall_type_t type;
        diy_syscall_param_t *param;
    } syscall;

    diy_thread_ctx context;
} diy_thread;

//ready queue for thread management
static struct {
    diy_thread *head;
    diy_thread *tail;
} readyque;

static diy_thread *current_thread;
static diy_thread threads[THREAD_NUM];
static diy_handler_t handlers[SOFTVEC_TYPE_NUM];
void dispatch(diy_thread_ctx *context);

static int get_current_thread(void){
    if(current_thread == NULL) return -1;
    readyque.head = current_thread->next;
    if(readyque.head == NULL) readyque.tail = NULL;
    current_thread->next = NULL;
    return 0;
}

static int put_current_thread(void){
    if(current_thread == NULL) return -1;

    //connect to tail of readyque
    if(readyque.tail){
        readyque.tail->next = current_thread;
    }
    else {
        readyque.head = current_thread;
    }
    readyque.tail = current_thread;
    return 0;
}

static void thread_end(void){
    diy_thread_exit();    
}

static void thread_init(diy_thread *thp){
    thp->init.func(thp->init.argc, thp->init.argv);
    thread_end();
}

static diy_thread_id_t thread_run(diy_func_t func, char *name, int stacksize, int argc, char *argv[]){
    int i;
    diy_thread *thp;
    uint32_t *sp;
    extern char userstack; //defined at ld.scr
    static char *thread_stack = &userstack;//acquire from userstack

    for(i = 0; i < THREAD_NUM; ++i){
        thp = &threads[i];
        if(!thp->init.func) break;
    }

    if(i == THREAD_NUM) return -1;//Not found

    memset(thp, 0, sizeof(*thp));

    //set Task Control Block (TCB)
    strcpy(thp->name, name);
    thp->next = NULL;
    thp->init.func = func;
    thp->init.argc = argc;
    thp->init.argv = argv;

    //acquire stack region and set it to TCB
    memset(thread_stack, 0, stacksize);
    thread_stack += stacksize;
    thp->stack = thread_stack;

    //set thread_end() as the return from  thread_init()
    sp = (uint32_t *)thp->stack;
    *(--sp) = (uint32_t)thread_end;

    //set thread_init() as the program counter at dispatch
    //thread starts from thread_init()
    *(--sp) = (uint32_t)thread_init;
    *(--sp) = 0;//ER6
    *(--sp) = 0;//ER5
    *(--sp) = 0;//ER4
    *(--sp) = 0;//ER3
    *(--sp) = 0;//ER2
    *(--sp) = 0;//ER1
    //variables to set at startup (thread_init())
    *(--sp) = (uint32_t)thp;//ER0

    //set thread context;
    thp->context.sp = (uint32_t)sp;
    //return syscalled thread to readyque
    put_current_thread();

    //insert new thread to readyque
    current_thread = thp;
    put_current_thread();

    return (diy_thread_id_t)current_thread;
}

static int thread_exit(void){
    //TODO: stack release operation is not implemented.
    //Then, thread create/delete cannot be done frequently.
    printf(current_thread->name);
    printf(" EXIT.\n");
    memset(current_thread, 0, sizeof(*current_thread));
    return 0;
}

static int setintr(softvec_type_t type, diy_handler_t handler){
    static void thread_intr(softvec_type_t type, uint32_t sp);
    //register interrupt handler to enter OS process
    softvec_setintr(type, thread_intr);
    // register interrupt handler for OS
    handlers[type] = handler;
    return 0;
}

static void call_functions(diy_syscall_type_t type, diy_syscall_param_t *param){
    switch(type){
        case DIY_SYSCALL_TYPE_RUN:
            param->un.run.ret = thread_run(param->un.run.func, param->un.run.name, param->un.run.stacksize,
                                            param->un.run.argc, param->un.run.argv);
            break;
        case DIY_SYSCALL_TYPE_EXIT:
            thread_exit();
            break;
        default:
            break;
    }
}

static void syscall_proc(diy_syscall_type_t type, diy_syscall_param_t *param){
    get_current_thread();//pop current_thread from readyque
    call_functions(type, param);
}

//thread scheduling
static void schedule(void){
    if(!readyque.head) diy_sysdown();//Not found
    current_thread = readyque.head;
}

static void syscall_intr(void){
    syscall_proc(current_thread->syscall.type, current_thread->syscall.param);
}

static void softerr_intr(void){
    printf(current_thread->name);
    printf(" DOWN.\n");
    //force exit if soft error occures
    get_current_thread();
    thread_exit();
}

static void thread_intr(softvec_type_t type, uint32_t sp){
    current_thread->context.sp = sp;

    //call handler if registered
    if(handlers[type]) handlers[type]();

    //schedule and dispatch thread.
    schedule();
    dispatch(&current_thread->context);
    //dispatch() does not return because the other thread starts working after dispatch().
}

void diy_start(diy_func_t func, char *name, int stacksize, int argc, char *argv[]){
    current_thread = NULL;
    readyque.head = readyque.tail = NULL;
    memset(threads, 0, sizeof(threads));
    memset(handlers, 0, sizeof(handlers));

    setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr);
    setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr);

    current_thread = (diy_thread *)thread_run(func, name, stacksize, argc, argv);
    dispatch(&current_thread->context);
}

void diy_sysdown(void){
    printf("system error!\n");
    while(1); //infinite loop.
}
void diy_syscall(diy_syscall_type_t type, diy_syscall_param_t *param){
    current_thread->syscall.type = type;
    current_thread->syscall.param = param;
    asm volatile ("trapa #0"); //issue trap interrupt
}

