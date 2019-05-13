#include <typedefs.h>
#include <diyos.h>
#include <intrdefs.h>
#include <interrupt.h>
#include <syscall.h>
#include <lib.h>
#include <memory.h>

#define THREAD_NUM 6
#define PRIORITY_NUM 16
#define THREAD_NAME_SIZE 15

//thread flags definition
#define DIY_THREAD_FLAG_READY  (1 << 0)

typedef struct diy_thread_ctx {
    uint32_t sp;//stack pointer
} diy_thread_ctx;

typedef struct diy_thread {
    struct diy_thread *next;//for thread task ready queue.
    char name[THREAD_NAME_SIZE + 1];
    int priority;
    char *stack;
    uint32_t flags;//for thread state
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

//message buffer and msgbox
typedef struct diy_msgbuf {
    struct diy_msgbuf *next;
    diy_thread *sender;
    struct {
        int size;
        char *p;
    } param;
} diy_msgbuf;

//msgbox

typedef struct diy_msgbox {
    diy_thread *receiver;
    diy_msgbuf *head;
    diy_msgbuf *tail;
    //Since H8 is 16bit CPU, 32bit MUL instruction is not available.
    //To avoid link errors like "__mulsi3 is not exist", arrange struct size is power of 2.
    long dummy[1];
} diy_msgbox;

//ready queue for thread management
static struct {
    diy_thread *head;
    diy_thread *tail;
} readyque[PRIORITY_NUM];

static diy_thread *current_thread;
static diy_thread threads[THREAD_NUM];
static diy_handler_t handlers[SOFTVEC_TYPE_NUM];
static diy_msgbox msgboxes[MSGBOX_ID_NUM];
void dispatch(diy_thread_ctx *context);

static int get_current_thread(void){
    if(current_thread == NULL) return -1;
    if(!(current_thread->flags & DIY_THREAD_FLAG_READY)){
        //Do nothing if thread is already not-ready.
        return 1;
    }
    int priority = current_thread->priority;
    readyque[priority].head = current_thread->next;
    if(readyque[priority].head == NULL) readyque[priority].tail = NULL;

    current_thread->flags &= ~DIY_THREAD_FLAG_READY;
    current_thread->next = NULL;
    return 0;
}

static int put_current_thread(void){
    if(current_thread == NULL) return -1;
    if(current_thread->flags & DIY_THREAD_FLAG_READY) return 1;

    //connect to tail of readyque
    int priority = current_thread->priority;
    if(readyque[priority].tail){
        readyque[priority].tail->next = current_thread;
    }
    else {
        readyque[priority].head = current_thread;
    }
    readyque[priority].tail = current_thread;
    current_thread->flags |= DIY_THREAD_FLAG_READY;
    return 0;
}

static void thread_end(void){
    diy_thread_exit();    
}

static void thread_init(diy_thread *thp){
    thp->init.func(thp->init.argc, thp->init.argv);
    thread_end();
}

static diy_thread_id_t thread_run(diy_func_t func, char *name, int priority, int stacksize, int argc, char *argv[]){
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
    thp->priority = priority;
    thp->flags = 0;
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
    //if priority is zero, start without interrupt.(0xC0)
    //thread starts from thread_init()
    *(--sp) = (uint32_t)thread_init | ((uint32_t) (priority ? 0: 0xC0) << 24);
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

static int thread_wait(void){
    //once remove from readyque and connect again to pass execution right.
    put_current_thread();
    return 0;
}

static int thread_sleep(void){
    //This call keeps the thread out of readyque.
    //That means the thread will not be scheduled.
    return 0;
}

static int thread_wakeup(diy_thread_id_t id){
    //called thread is restored to readyque
    put_current_thread();
    current_thread = (diy_thread *)id;
    put_current_thread();
    return 0;
}

static diy_thread_id_t thread_getid(void){
    put_current_thread();
    //Thread ID is Address of TCB.
    return (diy_thread_id_t)current_thread;
}

static int thread_change_priority(int priority){
    int prev_priority = current_thread->priority;
    if(priority >= 0) current_thread->priority = priority;
    put_current_thread();
    return prev_priority;
}

static void *thread_kmalloc(int size){
    put_current_thread();
    return diy_mem_alloc(size);
}

static int thread_kmfree(void *p){
    diy_mem_free(p);
    put_current_thread();
    return 0;
}

static void msgsend(diy_msgbox *pmsgbox, diy_thread *thp, int size, char *p){
    diy_msgbuf *pmsg;
    pmsg = (diy_msgbuf *)diy_mem_alloc(sizeof(*pmsg));
    if(pmsg == NULL) diy_sysdown();
    pmsg->next = NULL;
    pmsg->sender = thp;
    pmsg->param.size = size;
    pmsg->param.p = p;
    if(pmsgbox->tail != NULL){
        pmsgbox->tail->next = pmsg;
    }
    else {
        pmsgbox->head = pmsg;
    }
    pmsgbox->tail = pmsg;
}

static void msgrecv(diy_msgbox *pmsgbox){
    diy_msgbuf *pmsg;
    diy_syscall_param_t *param;
    pmsg = pmsgbox->head;
    pmsgbox->head = pmsg->next;
    if(pmsgbox->head == NULL){
        pmsgbox->tail = NULL;
    }
    pmsg->next = NULL;

    param = pmsgbox->receiver->syscall.param;
    param->un.msgrecv.ret = (diy_thread_id_t)pmsg->sender;
    if(param->un.msgrecv.psize != NULL){
        *(param->un.msgrecv.psize) = pmsg->param.size;
    }
    if(param->un.msgrecv.pp != NULL){
        *(param->un.msgrecv.pp) = pmsg->param.p;
    }
    pmsgbox->receiver = NULL;
    diy_mem_free(pmsg);
}

static int thread_msgsend(diy_msgbox_id_t id, int size, char *p){
    diy_msgbox *pmsgbox = &msgboxes[id];

    put_current_thread();//message send operation
    msgsend(pmsgbox, current_thread, size, p);

    //if waiting thread exists, do receive operation.
    if(pmsgbox->receiver != NULL){
        current_thread = pmsgbox->receiver;
        msgrecv(pmsgbox);
        put_current_thread();
    }

    return size;
}

static diy_thread_id_t thread_msgrecv(diy_msgbox_id_t id, int *psize, char **pp){
    diy_msgbox *pmsgbox = &msgboxes[id];
    if(pmsgbox->receiver != NULL) diy_sysdown();

    pmsgbox->receiver = current_thread;
    if(pmsgbox->head == NULL){
        //no message. sleep thread
        return -1;
    }

    msgrecv(pmsgbox);
    put_current_thread();//received. thread is ready.
    return current_thread->syscall.param->un.msgrecv.ret;
}

//interrupt handler registration at syscall
static int thread_setintr(softvec_type_t type, diy_handler_t handler){
    static void thread_intr(softvec_type_t type, uint32_t sp);
    //register interrupt handler to enter OS process
    softvec_setintr(type, thread_intr);
    // register interrupt handler for OS
    handlers[type] = handler;
    put_current_thread();//reentry to readyque
    return 0;
}

static void call_functions(diy_syscall_type_t type, diy_syscall_param_t *param){
    switch(type){
        case DIY_SYSCALL_TYPE_RUN:
            param->un.run.ret = thread_run(param->un.run.func, param->un.run.name, param->un.run.priority,
                                           param->un.run.stacksize, param->un.run.argc, param->un.run.argv);
            break;
        case DIY_SYSCALL_TYPE_EXIT:
            //do not write return value because TCB is deleted.
            thread_exit();
            break;
        case DIY_SYSCALL_TYPE_WAIT:
            param->un.wait.ret = thread_wait();
            break;
        case DIY_SYSCALL_TYPE_SLEEP:
            param->un.sleep.ret = thread_sleep();
            break;
        case DIY_SYSCALL_TYPE_WAKEUP:
            param->un.wakeup.ret = thread_wakeup(param->un.wakeup.id);
            break;
        case DIY_SYSCALL_TYPE_GETID:
            param->un.getid.ret = thread_getid();
            break;
        case DIY_SYSCALL_TYPE_CHANGE_PRIORITY:
            param->un.change_priority.ret = thread_change_priority(param->un.change_priority.priority);
            break;
        case DIY_SYSCALL_TYPE_KMALLOC:
            param->un.kmalloc.ret = thread_kmalloc(param->un.kmalloc.size);
            break;
        case DIY_SYSCALL_TYPE_KMFREE:
            param->un.kmfree.ret = thread_kmfree(param->un.kmfree.p);
            break;
        case DIY_SYSCALL_TYPE_MSGSEND:
            param->un.msgsend.ret = thread_msgsend(param->un.msgsend.id, param->un.msgsend.size, param->un.msgsend.p);
            break;
        case DIY_SYSCALL_TYPE_MSGRECV:
            param->un.msgrecv.ret = thread_msgrecv(param->un.msgrecv.id, param->un.msgrecv.psize, param->un.msgrecv.pp);
            break;
        case DIY_SYSCALL_TYPE_SETINTR:
            param->un.setintr.ret = thread_setintr(param->un.setintr.type, param->un.setintr.handler);
            break;
        default:
            break;
    }
}

static void syscall_proc(diy_syscall_type_t type, diy_syscall_param_t *param){
    get_current_thread();//pop current_thread from readyque
    call_functions(type, param);
}

static void srvcall_proc(diy_syscall_type_t type, diy_syscall_param_t *param){
    // Set current_thread NULL because if current_thread is referred in syscall, thread may work wrong.
    //srvcall should be called in the consective process of internal thread_intrvec().
    //current_thread is reset at that time.
    current_thread = NULL;
    call_functions(type, param);
}

//thread scheduling
static void schedule(void){
    int i;
    //lookup from priority 0
    for(i = 0; i < PRIORITY_NUM; ++i){
        if(readyque[i].head != NULL){
            break;
        }
    }
    if(i == PRIORITY_NUM) diy_sysdown();//Not found
    current_thread = readyque[i].head;
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
    //In the case of SOFTVEC_TYPE_SYSCALL, SOFTVEC_TYPE_SOFTERR,
    //syscall_intr(), soferr_intr() is called.
    //The other case is called the function which is registered by diy_setintr().
    if(handlers[type]) handlers[type]();

    //schedule and dispatch thread.
    schedule();
    dispatch(&current_thread->context);
    //dispatch() does not return because the other thread starts working after dispatch().
}

void diy_start(diy_func_t func, char *name, int priority, int stacksize, int argc, char *argv[]){
    diy_mem_init();//initialize memory for alloc/free operation
    current_thread = NULL;
    memset(readyque, 0, sizeof(readyque));
    memset(threads, 0, sizeof(threads));
    memset(handlers, 0, sizeof(handlers));
    memset(msgboxes, 0, sizeof(msgboxes));

    thread_setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr);
    thread_setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr);

    current_thread = (diy_thread *)thread_run(func, name, priority, stacksize, argc, argv);
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

void diy_srvcall(diy_syscall_type_t type, diy_syscall_param_t *param){
    srvcall_proc(type, param);    
}

