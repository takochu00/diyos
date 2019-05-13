#include <typedefs.h>
#include <diyos.h>
#include <intrdefs.h>
#include <interrupt.h>
#include <lib.h>
#include <console_driver.h>
#include <serial.h>

#define CONSOLE_BUFFER_SIZE 24

static struct consreg {
    diy_thread_id_t id;
    int index;
    char *send_buf;
    char *recv_buf;
    int send_len;
    int recv_len;
    long dummy[3];//adjust against compiler error
} consreg[CONSOLE_DRIVER_DEVICE_NUM];

//following two functions, send_char(), send_string() operates send buffer.
//Reentrant is prohibited. Then call with interrupt disabled if called by thread.

static void send_char(struct consreg *cons){
    int i;
    //send top of buffer and reduce length
    serial_send_byte(cons->index, cons->send_buf[0]);
    cons->send_len--;
    //shift one character.
    for(i = 0; i < cons->send_len; ++i){
        cons->send_buf[i] = cons->send_buf[i+1];
    }
}

static void send_string(struct consreg *cons, char *str, int len){
    int i;
    for(i = 0; i < len; ++i){
        if(str[i] == '\n'){
            cons->send_buf[cons->send_len++] = '\r';
        }
        cons->send_buf[cons->send_len++] = str[i];
    }

    if(cons->send_len && !serial_intr_is_send_enable(cons->index)){
        serial_intr_send_enable(cons->index);
        send_char(cons);
    }
    //else intr_send is enabled and buffers are sent in the process of send interrupt.
}


//following function is called by handler asynchronously.
//Usually, following limited functions can be called.
//reentrant, not called by thread, called with interrupt thread disabled
//And because this function is not in context state, syscall shall not be used.

static int console_driver_intrproc(struct consreg *cons){
    uint8_t c;
    char *p;
    if(serial_is_recv_enable(cons->index)){
        c = serial_recv_byte(cons->index);
        if(c == '\r'){
            c = '\n';
        }
        send_string(cons, &c, 1);//echo back

        if(cons->id){
            if(c != '\n'){
                cons->recv_buf[cons->recv_len++] = c;
            }
            else {
                p = diy_service_kmalloc(CONSOLE_BUFFER_SIZE);
                memcpy(p, cons->recv_buf, cons->recv_len);
                diy_service_msgsend(MSGBOX_ID_CONSOLE_INPUT, cons->recv_len, p);
                cons->recv_len = 0;
            }
        }
    }

    if(serial_is_send_enable(cons->index)){
        if(!cons->id || !cons->send_len){
            //no send data. Then exit.
            serial_intr_send_disable(cons->index);
        } else {
            send_char(cons);//output succesive character
        }
    }
    return 0;
}

static void console_driver_intr(void){
    int i;
    struct consreg *cons;
    for(i = 0; i < CONSOLE_DRIVER_DEVICE_NUM; ++i){
        cons = &consreg[i];
        if(cons->id){
            if(serial_is_send_enable(cons->index) || serial_is_recv_enable(cons->index)){
                console_driver_intrproc(cons);
            }
        }
    }
}

static int console_driver_init(void){
    memset(consreg, 0, sizeof(consreg));
    return 0;
}

static int console_driver_command(struct consreg *cons, diy_thread_id_t id, int index, int size, char *command){
    switch(command[0]){
        case CONSOLE_DRIVER_CMD_USE:
            cons->id = id;
            cons->index = command[1] - '0';
            cons->send_buf = diy_thread_kmalloc(CONSOLE_BUFFER_SIZE);
            cons->recv_buf = diy_thread_kmalloc(CONSOLE_BUFFER_SIZE);
            cons->send_len = 0;
            cons->recv_len = 0;
            serial_init(cons->index);
            serial_intr_recv_enable(cons->index);
            break;
        case CONSOLE_DRIVER_CMD_WRITE:
            INTR_DISABLE;//for exclude call
            send_string(cons, command + 1, size - 1);
            INTR_ENABLE;//release exclude call
            break;
        default:
            break;
    }
    return 0;
}

int console_driver_main(int argc, char *argv[]){
    int size, index;
    diy_thread_id_t id;
    char *p;
    console_driver_init();
    diy_thread_setintr(SOFTVEC_TYPE_SERINTR, console_driver_intr);

    while(1){
        id = diy_thread_msgrecv(MSGBOX_ID_CONSOLE_OUTPUT, &size, &p);
        index = p[0] - '0';
        console_driver_command(&consreg[index], id, index, size - 1, p + 1);
        diy_thread_kmfree(p);
    }
    return 0;
}
