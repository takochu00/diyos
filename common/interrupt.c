#include <interrupt.h>
#include <typedefs.h>
#include <intrdefs.h>

int softvec_init(void){
    int type;
    for(type = 0; type < SOFTVEC_TYPE_NUM; ++type){
        softvec_setintr(type, NULL);
    }
    return 0;
}

int softvec_setintr(softvec_type_t type, softvec_handler_t handler){
    if(type > SOFTVEC_TYPE_NUM) return -1;
    SOFTVECS[type] = handler;
    return 0;
}

void interrupt(softvec_type_t type, uint32_t sp){
    if(type > SOFTVEC_TYPE_NUM) return;
    softvec_handler_t handler = SOFTVECS[type];
    if(handler != NULL){
        handler(type, sp);
    }
}
