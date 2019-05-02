#include <typedefs.h>
#include <lib.h>
#include <serial.h>
#include <intrdefs.h>
#include <interrupt.h>

static void intr(softvec_type_t type, uint32_t sp){
    int c;
    static char buf[32];
    static int len;
    c = getchar();
    if(c != '\n'){
        buf[len++] = c;
    } else {
        //return
        buf[len++] = '\0';
        if(strncmp(buf, "echo", 4) == 0){
            printf(buf + 4);
            putchar('\n');
        }
        else {
            printf("unknown cmd\n");
        }
        printf("> ");
        len = 0;
    }
}

int main(void){

    printf("diyos loaded\n");

    softvec_setintr(SOFTVEC_TYPE_SERINTR, intr);
    serial_intr_recv_enable(SERIAL_DEFAULT_DEVICE);
    printf("> ");

    INTR_ENABLE;//enable interrupt
    while(1){
        //low power mode
        asm volatile ("sleep");
    }

    return 0;
}
